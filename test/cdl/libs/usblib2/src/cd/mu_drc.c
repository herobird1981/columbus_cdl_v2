/******************************************************************
*                                                                *
*        Copyright Mentor Graphics Corporation 2006              *
*                                                                *
*                All Rights Reserved.                            *
*                                                                *
*    THIS WORK CONTAINS TRADE SECRET AND PROPRIETARY INFORMATION *
*  WHICH IS THE PROPERTY OF MENTOR GRAPHICS CORPORATION OR ITS   *
*  LICENSORS AND IS SUBJECT TO LICENSE TERMS.                    *
*                                                                *
******************************************************************/

/*
 * Implementations of DRC-specific functions
 * $Revision: 1.77 $
 */

#include "mu_impl.h"
#include "mu_mem.h"

#ifdef MUSB_OTG

#include "mu_drcpr.h"
#include "mu_drcdf.h"

#include "mu_hstpr.h"
#include "mu_funpr.h"
#include "mu_pippr.h"
#include "mu_hdrdf.h"

#ifndef MUSB_VBUS_ERROR_RETRIES
#define MUSB_VBUS_ERROR_RETRIES 4
#endif

#ifndef MUSB_BABBLE_RETRIES
#define MUSB_BABBLE_RETRIES 4
#endif

/**************************** FORWARDS ****************************/

static uint8_t MGC_ExitingHost(uint8_t toOtg, MGC_Port* pPort);
static void MGC_ExitActiveUsbMode(uint8_t toOtg, MGC_Port* pPort);
static void MGC_StartPeripheralMode(MGC_Port* pPort);
static void MGC_AidlBdisTimeout(void* pParam, uint16_t wTimer);
static void MGC_WaitSessReqTimeout(void* pParam, uint16_t wTimer);
static void MGC_HostResetStart(void* pParam, uint16_t wTimer);
static void MGC_HostResetComplete(void* pParam, uint16_t wTimer);
static void MGC_DrcCompleteResume(void* pParam, uint16_t wTimer);
static void MGC_DrcFlushAll(MGC_Port* pPort);
static void MGC_SetHnpEnableIrpComplete(void* pCompleteParam, MUSB_ControlIrp* pIrp);
static void MGC_SetHnpSupportIrpComplete(void* pCompleteParam, MUSB_ControlIrp* pIrp);
static void MGC_OtgStateGetId(MGC_Port* pPort, uint8_t bIsReset);

/**************************** GLOBALS *****************************/

/** SET_FEATURE(a_hnp_support) request */
static const uint8_t MGC_aSetHnpSupportData[] =
{
    MUSB_DIR_OUT | MUSB_TYPE_STANDARD | MUSB_RECIP_DEVICE,
    MUSB_REQ_SET_FEATURE, 
    MUSB_a_hnp_support, 0,
    0, 0,
    0, 0
};

/** SET_FEATURE(b_hnp_enable) request */
static const uint8_t MGC_aSetHnpEnableData[] =
{
    MUSB_DIR_OUT | MUSB_TYPE_STANDARD | MUSB_RECIP_DEVICE,
    MUSB_REQ_SET_FEATURE, 
    MUSB_b_hnp_enable, 0,
    0, 0,
    0, 0
};

/**
 * IRP for setting a_hnp_support on root device.
 * NOTE: this singleton does not support multiple DRCs,
 * but the OTG standard doesn't really allow it anyway.
 */
static MUSB_ControlIrp MGC_SetHnpSupportIrp =
{
    0,	    /* fill bBusAddress */
    MGC_aSetHnpSupportData,
    sizeof(MGC_aSetHnpSupportData),
    NULL,   /* pInBuffer */
    0,	    /* dwInLength */
    0,	    /* dwStatus */
    0,	    /* dwActualOutLength */
    0,	    /* dwActualInLength */
    MGC_SetHnpSupportIrpComplete,
    NULL
};

/**
 * IRP for setting b_hnp_enable on root device.
 * NOTE: this singleton does not support multiple DRCs,
 * but the OTG standard doesn't really allow it anyway.
 */
static MUSB_ControlIrp MGC_SetHnpEnableIrp =
{
    0,	    /* fill bBusAddress */
    MGC_aSetHnpEnableData,
    sizeof(MGC_aSetHnpEnableData),
    NULL,   /* pInBuffer */
    0,	    /* dwInLength */
    0,	    /* dwStatus */
    0,	    /* dwActualOutLength */
    0,	    /* dwActualInLength */
    MGC_SetHnpEnableIrpComplete,
    NULL
};

/*************************** FUNCTIONS ****************************/

/*
 * Discover DRC FIFO configuration
 */
uint8_t MGC_DrcInit(MGC_Port* pPort)
{
    uint8_t bEnd;
    MGC_EndpointResource end;
    uint8_t bFifoSize;
    MGC_Controller* pImplController = pPort->pController;
    uint8_t* pBase = (uint8_t*)pImplController->pControllerAddressIst;
    uint8_t bOk = FALSE;

    /* count non-default endpoints by reading core */
    pPort->bEndCount = 1;
    pPort->wEndMask =  1;
    for(bEnd = 1; bEnd < MGC_MAX_USB_ENDS; bEnd++)
    {
	MGC_Write8(pBase, MGC_O_DRC_INDEX, bEnd);
	bFifoSize = MGC_Read8(pBase, MGC_O_DRC_FIFOSIZE);
	if(!bFifoSize)
	{
	    /* 0's returned when no more endpoints */
	    break;
	}
	pPort->bEndCount++;
	pPort->wEndMask |= (1 << bEnd);
    }

    pPort->pPrivateData = MUSB_MemAlloc(pPort->bEndCount *
	sizeof(MGC_EndpointResource));
    if(pPort->pPrivateData)
    {
	if(MUSB_ArrayInit(&(pPort->LocalEnds), sizeof(MGC_EndpointResource), 
	    pPort->bEndCount, pPort->pPrivateData))
	{
	    bOk = TRUE;

	    /* add endpoint 0 */
	    MUSB_MemSet((void*)&end, 0, sizeof(end));
	    end.bIsFifoShared = TRUE;
	    end.bTrafficType = end.bRxTrafficType = MUSB_ENDPOINT_XFER_CONTROL;
	    end.wMaxPacketSizeTx = MGC_END0_FIFOSIZE;
	    end.wMaxPacketSizeRx = MGC_END0_FIFOSIZE;
	    end.bIsClaimed = end.bRxClaimed = TRUE;
	    MUSB_ArrayAppend(&(pPort->LocalEnds), &end);

	    /* add others */
	    for(bEnd = 1; bEnd < pPort->bEndCount; bEnd++)
	    {
		MGC_Write8(pBase, MGC_O_DRC_INDEX, bEnd);
		bFifoSize = MGC_Read8(pBase, MGC_O_DRC_FIFOSIZE);
		MUSB_MemSet((void*)&end, 0, sizeof(end));
		end.bLocalEnd = bEnd;
		end.wMaxPacketSizeTx = 1 << (bFifoSize & 0x0f);
		/* shared TX/RX FIFO? */
		if((bFifoSize & 0xf0) == 0xf0)
		{
	    	    end.wMaxPacketSizeRx = 1 << (bFifoSize & 0x0f);
		    end.bIsFifoShared = TRUE;
		}
		else
		{
		    end.wMaxPacketSizeRx = 1 << ((bFifoSize & 0xf0) >> 4);
		    end.bIsFifoShared = FALSE;
		}
		MUSB_ArrayAppend(&(pPort->LocalEnds), &end);
	    }
	}
	else
	{
	    MUSB_MemFree(pPort->pPrivateData);
	}
    }

    /* select endpoint 0 */
    MGC_Write8(pBase, MGC_O_DRC_INDEX, 0);

    return bOk;
}

/*
 * DRC Interrupt Service Routine
 */
int MGC_DrcIsr(MGC_Controller* pController, uint8_t bIntrUsbValue,
	       uint16_t wIntrTxValue, uint16_t wIntrRxValue)
{
    MGC_BsrItem qItem;
    uint32_t dwRegVal;
    uint8_t bQueue;
    unsigned int iShift;
    int iResult = -1;
    MUSB_SystemServices* pServices = pController->pSystemServices;
    MGC_Port* pPort = pController->pPort;

#ifdef MUSB_DMA
    pPort->bDmaCompleted = FALSE;
    if(pPort->pDmaController && pPort->pDmaController->pfDmaControllerIsr)
    {
	if(pPort->pDmaController->pfDmaControllerIsr(
	    pPort->pDmaController->pPrivateData))
	{
	    iResult = pPort->bDmaCompleted;
	}
    }
#endif	/* DMA enabled */

    if(bIntrUsbValue)
    {
	MGC_DIAG1(3, pController, "IntrUSB=", bIntrUsbValue, 16, 2);
	iResult = MGC_DrcUsbIsr(pPort, bIntrUsbValue);
    }
    MGC_DIAG2(3, pController, "IntrTx=", wIntrTxValue, " / IntrRx=", 
	wIntrRxValue, 16, 4);

    /* scan for Tx endpoints requiring servicing */
    dwRegVal = wIntrTxValue;
    if(dwRegVal && (iResult < 0))
    {
	iResult = 0;
    }
    /* the low bit is a special case: Tx or Rx on endpoint 0 */
    if(dwRegVal & 1)
    {
	bQueue = pPort->pfServiceDefaultEnd(pPort, &qItem);
	if(bQueue)
	{
	    iResult++;
#ifdef MUSB_OVERHEAD
	    qItem.dwTime = pController->pUtils->pfGetTime();
#endif
	    pServices->pfQueueBackgroundItem(pServices->pPrivateData, &qItem);
	}
    }

    /* scan for Tx endpoints 1-15 requiring servicing */
    iShift = 1;
    dwRegVal >>= 1;
    while(dwRegVal)
    {
	if(dwRegVal & 1)
	{
	    bQueue = pPort->pfServiceTransmitAvail(pPort, iShift, &qItem);
	    if(bQueue)
	    {
		iResult++;
#ifdef MUSB_OVERHEAD
		qItem.dwTime = pController->pUtils->pfGetTime();
#endif
		qItem.bLocalEnd = iShift;
		pServices->pfQueueBackgroundItem(pServices->pPrivateData, &qItem);
	    }
	}
	dwRegVal >>= 1;
	iShift++;
    }

    /* scan for Rx endpoints 1-15 requiring servicing */
    dwRegVal = wIntrRxValue;
    if(dwRegVal && (iResult < 0))
    {
	iResult = 0;
    }
    iShift = 1;
    dwRegVal >>= 1;
    while(dwRegVal)
    {
	if(dwRegVal & 1)
	{
	    bQueue = pPort->pfServiceReceiveReady(pPort, iShift, &qItem);
	    if(bQueue)
	    {
		iResult++;
#ifdef MUSB_OVERHEAD
		qItem.dwTime = pController->pUtils->pfGetTime();
#endif
		qItem.bLocalEnd = iShift;
		pServices->pfQueueBackgroundItem(pServices->pPrivateData, &qItem);
	    }
	}
	dwRegVal >>= 1;
	iShift++;
    }

    return iResult;
}

/*
* Find the DRC's first (host) or required (function) 
* local endpoint resource to communicate with the given remote endpoint (host)
* or to be identified with the given endpoint descriptor (function).
*/
MGC_EndpointResource* MGC_DrcBindEndpoint(MGC_Port* pPort, 
					  const MUSB_DeviceEndpoint* pUsbEnd,
					  MUSB_EndpointResource* pRequest,
					  uint8_t bBind, uint8_t* pbIsTx)
{
    uint32_t dwEndCount;
    uint32_t dwEndIndex;
    uint8_t bIsIn;
    uint8_t bIsTx;
    uint8_t bEnd;
    uint8_t bIsClaimed;
    uint16_t wPacketSize;
    MGC_EndpointResource* pEnd;
    int32_t dwDiff;
    int32_t dwReqDiff;
    uint16_t wMaxPacketSize;
    MGC_EndpointResource* pBestEnd = NULL;
    MGC_EndpointResource* pBestReqEnd = NULL;
    uint16_t wBestDiff = 0xffff;
    uint16_t wBestReqDiff = 0xffff;

    wPacketSize = MUSB_SWAP16P((uint8_t*)&(pUsbEnd->UsbDescriptor.wMaxPacketSize));
    bIsIn = (pUsbEnd->UsbDescriptor.bEndpointAddress & MUSB_ENDPOINT_DIR_MASK) ? TRUE : FALSE;
    bIsTx = pPort->bIsHost ? !bIsIn : bIsIn;
    dwEndCount = MUSB_ArrayLength(&(pPort->LocalEnds));
    for(dwEndIndex = 0; dwEndIndex < dwEndCount; dwEndIndex++)
    {
	pEnd = (MGC_EndpointResource*)MUSB_ArrayFetch(&(pPort->LocalEnds), dwEndIndex);
	bIsClaimed = pEnd->bIsFifoShared ? 
	    pEnd->bIsClaimed : (bIsTx ? pEnd->bIsClaimed : pEnd->bRxClaimed);
	if(pEnd && !bIsClaimed)
	{
	    wMaxPacketSize = bIsTx ? pEnd->wMaxPacketSizeTx : pEnd->wMaxPacketSizeRx;
	    if(pPort->bIsHost)
	    {
		/* host role: need compatible direction and try for closest FIFO size match */
		dwDiff = wMaxPacketSize - wPacketSize;
		dwReqDiff = wMaxPacketSize - pRequest->dwBufferSize;
		if(dwDiff >= 0)
		{
		    /* found candidate; see if it is the best thus far */
		    if((uint16_t)dwDiff < wBestDiff)
		    {
			/* best */
			wBestDiff = (uint16_t)dwDiff;
			pBestEnd = pEnd;
		    }
		    /* also, see how close we are to properties requested */
		    if((uint16_t)dwReqDiff < wBestReqDiff)
		    {
			/* best */
			wBestReqDiff = (uint16_t)dwReqDiff;
			pBestReqEnd = pEnd;
		    }
		}
	    }
	    else
	    {
		/* function role: need address match, compatible direction and FIFO size */
		bEnd = pUsbEnd->UsbDescriptor.bEndpointAddress & MUSB_ENDPOINT_NUMBER_MASK;
		if((wMaxPacketSize >= wPacketSize) &&
		    (pEnd->bLocalEnd == bEnd))
		{
		    /* found match */
		    pBestEnd = pEnd;
		    break;
		}
	    }
	}
    }

    /* see if we fulfilled desire */
    if(pBestReqEnd)
    {
	pBestEnd = pBestReqEnd;
    }

    /* if we found one, set it up */
    if(pBestEnd && bBind)
    {
	if(pBestEnd->bIsFifoShared || bIsTx)
	{
	    pBestEnd->bIsClaimed = TRUE;
	}
	else
	{
	    pBestEnd->bRxClaimed = TRUE;
	}
	pBestEnd->bIsTx = bIsTx;
	pBestEnd->bIsHalted = FALSE;
	if(bIsTx)
	{
	    pBestEnd->bBusAddress = pUsbEnd->pDevice->bBusAddress;
	    pBestEnd->bBusEnd = pUsbEnd->UsbDescriptor.bEndpointAddress;
	    pBestEnd->bInterval = pUsbEnd->UsbDescriptor.bInterval;
	    pBestEnd->bTrafficType = pUsbEnd->UsbDescriptor.bmAttributes;
	    pBestEnd->wPacketSize = wPacketSize;
	}
	else
	{
	    pBestEnd->bRxBusAddress = pUsbEnd->pDevice->bBusAddress;
	    pBestEnd->bRxBusEnd = pUsbEnd->UsbDescriptor.bEndpointAddress;
	    pBestEnd->bRxInterval = pUsbEnd->UsbDescriptor.bInterval;
	    pBestEnd->bRxTrafficType = pUsbEnd->UsbDescriptor.bmAttributes;
	    pBestEnd->wRxPacketSize = wPacketSize;
	}
    }

    *pbIsTx = bIsTx;
    return pBestEnd;

}

/*
 * Handle default endpoint interrupt
 */
uint8_t MGC_DrcServiceDefaultEnd(MGC_Port* pPort, MGC_BsrItem* pItem, 
				 uint16_t* pwCsr, uint16_t wCount)
{
    uint16_t wFifoCount, wSpace;
    uint8_t* pFifoDest;
    MUSB_DeviceRequest* pRequest;
    uint8_t bVal = (uint8_t)*pwCsr;
    uint8_t bOutVal = 0;
    uint8_t bResult = FALSE;
    uint8_t status = MUSB_STATUS_OK;
    MGC_EndpointResource* pEnd = (MGC_EndpointResource*)MUSB_ArrayFetch(&(pPort->LocalEnds), 0);

    if(pPort->bIsHost)
    {
	if(status || (pEnd->bIsTx && !pEnd->dwTxSize) || (!pEnd->bIsTx && !wCount))
	{
	    /* assume status phase complete */
	    *pwCsr = 0;
	    MGC_CompleteIrp(pItem, pEnd, (uint8_t)status, TRUE, pEnd->pTxIrp);
	    /* start next transfer if any */
	    MGC_StartNextControlTransfer(pPort);
	    return TRUE;
	}

	/* call common logic and prepare response */
	if(!MGC_DrcServiceHostDefault(pPort, wCount, status, &bOutVal))
	{
	    /* data transfer complete; perform status phase */
	    bOutVal = MGC_M_CSR0_H_STATUSPKT;
	    if(pEnd->bIsTx)
	    {
		pEnd->bIsTx = FALSE;
		bOutVal |= MGC_M_CSR0_H_REQPKT;
	    }
	    else
	    {
		pEnd->bIsTx = TRUE;
		pEnd->dwTxSize = 0;
		bOutVal |= MGC_M_CSR0_TXPKTRDY;
	    }
	}
    }
    else
    {
	/* peripheral mode: see if Rx or Tx */
	if(bVal & MGC_M_CSR0_RXPKTRDY)
	{
	    pPort->bEnd0Status = FALSE;
	    /* receive */
	    wSpace = pPort->pFunctionClient ? 
		(pPort->pFunctionClient->wControlBufferLength - pPort->wSetupDataIndex) : 0;
	    wFifoCount = MUSB_MIN(wCount, wSpace);
	    /* stall on potential overflow or get data */
	    if(!pPort->pFunctionClient || (wCount > wFifoCount))
	    {
		/* stall */
		bOutVal |= MGC_M_CSR0_P_SENDSTALL;
	    }
	    else
	    {
		/* unload FIFO to the right place */
		pFifoDest = pPort->pFunctionClient->pControlBuffer + pPort->wSetupDataIndex;
#ifdef MUSB_DMA
		if(pEnd->pDmaChannel && pPort->pDmaController->pfDmaProgramChannel(
		    pEnd->pDmaChannel, pEnd->wPacketSize, pEnd->pDmaChannel->bDesiredMode, 
		    pFifoDest, 
		    MGC_MIN(wFifoCount, pEnd->pDmaChannel->dwMaxLength)))
		{
		    /* prevent caller touching CSR0 */
		    *pwCsr = 0;
		    return TRUE;
		}
#endif
		pPort->pfUnloadFifo(pPort, 0, wFifoCount, pFifoDest);
		pPort->wSetupDataIndex += wFifoCount;
		/* check request */
		pRequest = (MUSB_DeviceRequest*)pPort->pFunctionClient->pControlBuffer;
		/* if IN or we got enough OUT data, parse */
		if((pRequest->bmRequestType & MUSB_DIR_IN) || 
		    ((pPort->wSetupDataIndex - 8) >= MUSB_SWAP16P((uint8_t*)&(pRequest->wLength))))
		{
		    pPort->dwSequenceNumber++;
		    pPort->wSetupDataSize = pPort->wSetupDataIndex;
		    pPort->wSetupDataIndex = 0;
		    bResult = FALSE;
		    /* if parsing enabled */
		    if(pPort->bParse)
		    {
			/* try to handle it now */
			if(!MGC_FunctionParseSetup(pPort, &status))
			{
			    /* callback */
			    if(!pPort->pFunctionClient->pfDeviceRequest(
				pPort->pFunctionClient->pPrivateData, (MUSB_BusHandle)pPort,
				pPort->dwSequenceNumber, pPort->pFunctionClient->pControlBuffer,
				pPort->wSetupDataIndex))
			    {
				/* client rejected immediately, so STALL */
				pPort->pfDefaultEndResponse(pPort, TRUE);
			    }
			}
			else
			{
			    /* parser handled it */
			    if(MUSB_STATUS_STALLED == status)
			    {
				pPort->wSetupDataIndex = 0;
				pPort->wSetupDataSize = 0;
				pPort->pfDefaultEndResponse(pPort, TRUE);
			    }
			    else
			    {
				pPort->pfDefaultEndResponse(pPort, FALSE);
			    }
			}
		    }
		    else
		    {
			/* parsing disabled; issue callback */
			if(!pPort->pFunctionClient->pfDeviceRequest(
			    pPort->pFunctionClient->pPrivateData, (MUSB_BusHandle)pPort,
			    pPort->dwSequenceNumber, pPort->pFunctionClient->pControlBuffer,
			    pPort->wSetupDataIndex))
			{
			    /* client rejected immediately, so STALL */
			    pPort->pfDefaultEndResponse(pPort, TRUE);
			}
		    }
		}
		else
		{
		    /* acknowledge partial message */
		    bOutVal = MGC_M_CSR0_P_SVDRXPKTRDY;
		}
	    }
	}
	else if(!(bVal & MGC_M_CSR0_TXPKTRDY))
	{
	    /* transmit-ready */
	    wFifoCount = MUSB_MIN(64, (pPort->wSetupDataSize - pPort->wSetupDataIndex));
	    if(!wFifoCount)
	    {
		/* status ack */
		pPort->wSetupDataIndex = 0;
		pPort->wSetupDataSize = 0;
		return FALSE;
	    }
	    pFifoDest = pPort->pSetupData + pPort->wSetupDataIndex;
#ifdef MUSB_DMA
	    if(pEnd->pDmaChannel && pPort->pDmaController->pfDmaProgramChannel(
		pEnd->pDmaChannel, pEnd->wPacketSize, pEnd->pDmaChannel->bDesiredMode, 
		pFifoDest, 
		MGC_MIN(wFifoCount, pEnd->pDmaChannel->dwMaxLength)))
	    {
		/* prevent caller touching CSR0 */
		*pwCsr = 0;
		return TRUE;
	    }
#endif
	    /* load and signal need to transmit */
	    pPort->pfLoadFifo(pPort, 0, wFifoCount, pFifoDest);
	    pPort->wSetupDataIndex += wFifoCount;
	    bOutVal |= MGC_M_CSR0_TXPKTRDY;
	    if(pPort->wSetupDataIndex >= pPort->wSetupDataSize)
	    {
		bOutVal |= MGC_M_CSR0_P_DATAEND;
		pPort->wSetupDataIndex = 0;
		pPort->wSetupDataSize = 0;
	    }
	}
    }

    *pwCsr = bOutVal;
    return bResult;
}

/* Reset the port (e.g. unbind/flush all endpoints, etc.) */
uint32_t MGC_DrcResetPort(MGC_Port* pPort)
{
    MGC_Controller* pController = pPort->pController;

    /* kill timer */
    pController->pSystemServices->pfCancelTimer(
	pController->pSystemServices->pPrivateData, 0);

    /* turn off session */
    pPort->bWantSession = FALSE;
    pPort->bFuncAddr = 0;
    pPort->pfProgramBusState(pPort);

    /* stop controller */
    pController->pfProgramStopController(pController);

    /* flush BSR queue */
    pController->pSystemServices->pfFlushBackgroundQueue(
	pController->pSystemServices->pPrivateData);

    MGC_DrcFlushAll(pPort);

    pController->pfProgramStartController(pController);

    /* get current readings */
    pPort->pfReadBusState(pPort);

    pPort->bWantReset = FALSE;
    pPort->bUsbState = MUSB_POWER_OFF;
    pPort->bWantHost = FALSE;
    
    pPort->bOtgState = MUSB_AB_IDLE;
    MGC_CompleteOtgTransition(pPort);

    return 0;
}

/**
 * Stop all traffic.
 * Flush all local endpoints, kill all pending IRPs
 * @param pPort port pointer
 */
static void MGC_DrcFlushAll(MGC_Port* pPort)
{
    uint32_t dwEndCount;
    uint32_t dwEndIndex;
    uint16_t wCount;
    uint16_t wIndex;
    void* pIrp;
    MGC_EndpointResource* pEnd;
    uint8_t* pBase = (uint8_t*)pPort->pController->pControllerAddressIst;

    /* flush all endpoints */
    dwEndCount = MUSB_ArrayLength(&(pPort->LocalEnds));
    for(dwEndIndex = 1; dwEndIndex < dwEndCount; dwEndIndex++)
    {
	pEnd = (MGC_EndpointResource*)MUSB_ArrayFetch(&(pPort->LocalEnds), dwEndIndex);
	if(pEnd)
	{
	    pEnd->bIsClaimed = pEnd->bRxClaimed = FALSE;
	    pEnd->bIsHalted = pEnd->bIsRxHalted = FALSE;
	    pEnd->pTxIrp = pEnd->pRxIrp = NULL;
#ifdef MUSB_DMA
	    pEnd->bDmaTx = pEnd->bDmaResidual = FALSE;
#endif
	    pPort->pfProgramFlushEndpoint(pPort, pEnd, 0, TRUE);
	    pPort->pfProgramFlushEndpoint(pPort, pEnd, MUSB_ENDPOINT_DIR_MASK, TRUE);
	    wCount = MUSB_ListLength(&(pEnd->TxIrpList));
	    for(wIndex = 0; wIndex < wCount; wIndex++)
	    {
		pIrp = MUSB_ListFindItem(&(pEnd->TxIrpList), 0);
		MUSB_ListRemoveItem(&(pEnd->TxIrpList), pIrp);
	    }
	    wCount = MUSB_ListLength(&(pEnd->RxIrpList));
	    for(wIndex = 0; wIndex < wCount; wIndex++)
	    {
		pIrp = MUSB_ListFindItem(&(pEnd->RxIrpList), 0);
		MUSB_ListRemoveItem(&(pEnd->RxIrpList), pIrp);
	    }
	}
    }

    /* select endpoint 0 */
    MGC_Write8(pBase, MGC_O_DRC_INDEX, 0);

}

/* OTG timer expiration */
void MGC_DrcOtgTimer(void* pParam, uint16_t wTimer)
{
    MGC_Controller* pController = (MGC_Controller*)pParam;
    MGC_Port* pPort = pController->pPort;
    MUSB_OtgClient* pOtgClient = (MUSB_OtgClient*)pPort->pOtgClient;

    pPort->pfReadBusState(pPort);
    switch(pPort->bOtgState)
    {
    case MUSB_B_WAIT_ACON:
	if(pPort->bBusVoltage < MGC_VBUS_ABOVE_AVALID)
	{
	    MGC_DIAG(1, pPort->pController, "DEVICE NOT RESPONDING (A-device didn't connect)");
	    /* signal error */
	    if(pOtgClient) pOtgClient->pfOtgError(pOtgClient->pPrivateData,
		pOtgClient, MUSB_STATUS_OTG_NO_RESPONSE);
	    /* back to idle state */
	    MGC_DrcChangeOtgState(MUSB_AB_IDLE, pPort);
	}
	break;

    case MUSB_B_PERIPHERAL:
    case MUSB_B_HOST:
	if(pPort->bBusVoltage < MGC_VBUS_ABOVE_AVALID)
	    MGC_DrcChangeOtgState(MUSB_AB_IDLE, pPort);
    }

}   /* MGC_DrcOtgTimer */

/*
 * @return TRUE if leaving an active host mode, else FALSE.
 */
static uint8_t MGC_ExitingHost(uint8_t toOtg, MGC_Port* pPort)
{
    if(MUSB_A_SUSPEND == toOtg)
    {
	/* A_SUSPEND itself is not exiting host */
	return(FALSE);
    }
    else if(MUSB_A_PERIPHERAL == toOtg)
    {
	/* Only way here is from a host(suspend) */
	return(TRUE);
    }
    else if((MUSB_A_HOST == pPort->bOtgState) || (MUSB_B_HOST == pPort->bOtgState))
    {
	return(TRUE);
    }
    return(FALSE);
}   /* MGC_ExitingHost */

/*
* Flush transfer requests, reset endpoint variables,
* reset DRC registers when exiting an active USB data mode.
*
* Current OTG state is pPort->bOtgState; changing to "toOtg".
* If "toOtg" an HNP change, leave DRC registers alone.
*/
static void MGC_ExitActiveUsbMode(uint8_t toOtg, MGC_Port* pPort)
{
    MGC_FunctionChangeState(pPort, MUSB_POWERED);

    MGC_DrcFlushAll(pPort);

    /* reset USB address value */
    pPort->bFuncAddr = 0;
    pPort->pfProgramBusState(pPort);

}   /* MGC_ExitActiveUsbMode */

static void MGC_SetPowerSave(MGC_Port* pPort, uint8_t bSave)
{
	/*
	pPort->pfReadBusState(pPort);
	pPort->bWantSuspend = bSave;
	pPort->bWantSession = pPort->bIsSession;
    pPort->pfProgramBusState(pPort);
	*/
}

/*
*  MGC_Start_Peripheral_Mode used to condition DRC for peripheral operation
*  and establish peripheral mode in the core & endpoint objects.
*/
static void MGC_StartPeripheralMode(MGC_Port* pPort)
{
	/*
	MGC_SetPowerSave(pPort, TRUE);
	*/
    /* allow us to be suspended */
	/*
    pPort->bWantSuspend = TRUE;
    pPort->bWantSession = TRUE;
    pPort->pfProgramBusState(pPort);
	*/

}   /* MGC_StartPeripheralMode */

/*
*  Cancel an A-dev bus suspend, inviting B-device to become host.
*/
static void MGC_AidlBdisTimeout(void* pParam, uint16_t wTimer)
{
    MGC_Controller* pController = (MGC_Controller*)pParam;
    MGC_Port* pPort = pController->pPort;
    MUSB_OtgClient* pOtgClient = (MUSB_OtgClient*)pPort->pOtgClient;

	return;  /*sunny modify for hnp test*/

    if  (pPort->bIsSuspend || pPort->bIsReset)
    {
	return;
    }
    MGC_DIAG(1, pPort->pController, "DEVICE NOT RESPONDING (did not HNP)");

    /* signal error */
    if(pOtgClient) pOtgClient->pfOtgError(pOtgClient->pPrivateData, pOtgClient,
	MUSB_STATUS_OTG_NO_RESPONSE);

    /* change to idle (OTG 6.8.1.5) */
    MGC_DrcChangeOtgState(MUSB_AB_IDLE, pPort);

}   /* MGC_AidlBdisTimeout */

/*
*  MGC_WaitSessReqTimeout used to time out following writing a 
*  SESSREQ to DRC.  Could result in PERIPHERAL mode(via SRP), or
*  HOST.  If neither, exit back to IDLE.
*/
static void MGC_WaitSessReqTimeout(void* pParam, uint16_t wTimer)
{
    MGC_Controller* pController = (MGC_Controller*)pParam;
    MGC_Port* pPort = pController->pPort;
    MUSB_OtgClient* pOtgClient = (MUSB_OtgClient*)pPort->pOtgClient;

    if  (pPort->bIsSuspend || pPort->bIsReset)
    {
	return;
    }
    MGC_DIAG(1, pPort->pController, "DEVICE NOT RESPONDING");

    /* signal error */
    if(pOtgClient) pOtgClient->pfOtgError(pOtgClient->pPrivateData, pOtgClient,
	MUSB_STATUS_OTG_NO_RESPONSE);

    /* change to idle */
    MGC_DrcChangeOtgState(MUSB_AB_IDLE, pPort);

}   /* MGC_WaitSessReqTimeout */

/*
*  MGC_HostResetStart allows us to stall slightly the start of
*  the RESET of the B-device or USB peripheral.  This is needed
*  for HNP following a SUSPEND by a B-HOST condition.  
*/
static void MGC_HostResetStart(void* pParam, uint16_t wTimer)
{
    MGC_Controller* pController = (MGC_Controller*)pParam;
    MGC_Port* pPort = pController->pPort;

    MGC_DIAG(3, pController, "Host Start reset signal");
    MGC_DIAG(2, pController, "MGC_HostResetStart");

    pPort->bWantReset = TRUE;
    pPort->bWantSuspend = FALSE;
    pPort->pfProgramBusState(pPort);

    pController->pSystemServices->pfArmTimer(
	pController->pSystemServices->pPrivateData, 0, 10, FALSE,
	MGC_HostResetComplete);

}   /* MGC_HostResetStart */

/*
*  MGC_HostResetComplete to commence enumeration, following USB RESET
*/
static void MGC_HostResetComplete(void* pParam, uint16_t wTimer)
{
    uint8_t bAddress;
    uint8_t bSpeed = MUSB_CONNECTION_SPEED_FULL;
    MGC_Controller* pController = (MGC_Controller*)pParam;
    MGC_Port* pPort = pController->pPort;

    MGC_DIAG(3, pController, "Host Stop reset signal");
    MGC_DIAG(2, pController, "MGC_HostResetComplete");

    pPort->bWantReset = FALSE;
    pPort->pfProgramBusState(pPort);

    pPort->pfReadBusState(pPort);
    if(pPort->bIsHighSpeed)
    {
	bSpeed = MUSB_CONNECTION_SPEED_HIGH;
    }
    else if(pPort->bIsLowSpeed)
    {
	bSpeed = MUSB_CONNECTION_SPEED_LOW;
    }

    /* start enumeration */
#ifdef SET_ADDR_FIRST
    bAddress = MGC_AllocateAddress(&(pPort->EnumerationData));
    if(bAddress)
    {
	if(!MGC_EnumerateDevice(pPort, NULL, bAddress, 0, bSpeed, NULL))
	{
	    MGC_DIAG(1, pController, "Insufficient memory for enumeration");
	}
    }
    else
    {
	MGC_DIAG(1, pController, "Insufficient addresses for enumeration");
    }
#else
    bAddress = 0;
    if(!MGC_EnumerateDevice(pPort, NULL, bAddress, 0, bSpeed, NULL))
    {
        MGC_DIAG(1, pController, "Insufficient memory for enumeration");
    }
#endif
}   /* MGC_HostResetComplete */

/*
* A timer started when entering b_srp_init reached a vendor specfic limit
* between 5 ~ 30 sec. If failed to enter b_peripheral, it will indicate 
* to the user that the A-device did not respond.
*/
static void MGC_BSrpFailed(void* pParam, uint16_t wTimer)
{
    MGC_Controller* pController = (MGC_Controller*)pParam;
    MGC_Port* pPort = pController->pPort;
    MUSB_OtgClient* pOtgClient = (MUSB_OtgClient*)pPort->pOtgClient;

    if((MUSB_B_SRP_INIT == pPort->bOtgState) || 
	(MUSB_AB_IDLE == pPort->bOtgState))
    {
	/* 
	* if b_sess_vld is true, we should be in B_PERIPHERAL state
	*/
	if(pPort->bBusVoltage >= MGC_VBUS_ABOVE_AVALID)
	{
	    MGC_DIAG(1, pPort->pController,
		"B_SRP_Fail: changing to B_PERIPHERAL");

	    /* signal error */
	    if(pOtgClient) pOtgClient->pfOtgError(pOtgClient->pPrivateData,
		pOtgClient, MUSB_STATUS_OTG_SRP_FAILED);
	}
	else
	{
	    /* clear sess & hr requests */
	    pPort->bWantSession = FALSE;
	    pPort->bWantHost = FALSE;
	    pPort->pfProgramBusState(pPort);

	    MGC_DIAG(1, pPort->pController, "DEVICE NOT RESPONDING (SRP)");

	    /* signal error */
	    if(pOtgClient) pOtgClient->pfOtgError(pOtgClient->pPrivateData,
		pOtgClient, MUSB_STATUS_OTG_NO_RESPONSE);

	    /* change to idle */
	    MGC_DrcChangeOtgState(MUSB_AB_IDLE, pPort);   /* go back to quiescence */
	}
    }
}   /* MGC_BSrpFail */

/** Timer B_ASE0_BRST expiration callback */
static void MGC_BAase0BrstTimeout(void* pParam, uint16_t wTimer)
{
    MGC_Controller* pController = (MGC_Controller*)pParam;
    MGC_Port* pPort = pController->pPort;
    MUSB_OtgClient* pOtgClient = (MUSB_OtgClient*)pPort->pOtgClient;

    if(MUSB_B_WAIT_ACON == pPort->bOtgState)
    {
	MGC_DIAG(1, pPort->pController, "DEVICE NOT RESPONDING (A-device didn't connect)");

	/* signal error */
	if(pOtgClient) pOtgClient->pfOtgError(pOtgClient->pPrivateData,
	    pOtgClient, MUSB_STATUS_OTG_NO_RESPONSE);

	/* clear host request */
	pPort->bWantHost = FALSE;
	pPort->pfProgramBusState(pPort);

	MGC_DrcChangeOtgState(MUSB_B_PERIPHERAL, pPort);

    }
}

/** IRP completion callback for the IRP to SET_FEATURE(a_hnp_support) */
static void MGC_SetHnpSupportIrpComplete(void* pCompleteParam, MUSB_ControlIrp* pIrp)
{
    /* TODO: anything? */
}

/** IRP completion callback for the IRP to SET_FEATURE(b_hnp_enable) */
static void MGC_SetHnpEnableIrpComplete(void* pCompleteParam, MUSB_ControlIrp* pIrp)
{
    MGC_Port* pPort = (MGC_Port*)pCompleteParam;
    MUSB_SystemServices* pServices = pPort->pController->pSystemServices;

    if(MUSB_STATUS_OK == pIrp->dwStatus)
    {
		info("A-HOST: SET_FEATURE(b_hnp_enable), suspend the bus, change to MUSB_A_SUSPEND\n");
		pPort->bWantSuspend = TRUE;
		pPort->pfProgramBusState(pPort);
		pServices->pfArmTimer(pServices->pPrivateData, 0, MGC_TA_AIDL_BDIS,
			FALSE, MGC_AidlBdisTimeout);
    }
	else
	{
		MGC_SetPowerSave(pPort, TRUE);
	}
}

static void MGC_RemoveSuspend(void* pParam, uint16_t wTimer)
{
    MGC_Controller* pController = (MGC_Controller*)pParam;
    MGC_Port* pPort = pController->pPort;

    pPort->bWantSuspend = FALSE;
    pPort->pfProgramBusState(pPort);
}

/*
*  MGC_DrcChangeOtgState effects the changes to the OTG mode paradigms (wha?)
*/
void MGC_DrcChangeOtgState(uint8_t toOtg, MGC_Port* pPort)
{
    const MUSB_OtgDescriptor* pOtgDesc;
    MUSB_SystemServices* pServices = pPort->pController->pSystemServices;

#if MUSB_DIAG > 1
    pPort->pController->pfDumpControllerState(pPort->pController);
#endif

    pPort->pfReadBusState(pPort);

    /* kill scheduled event */
    pServices->pfCancelTimer(pServices->pPrivateData, 0);

    /*
     *  If exiting peripheral or host, release held objects
     */
    if((MGC_ExitingHost(toOtg, pPort)) || (MGC_AB_MODE(pPort->bOtgState) == MGC_AB_PERIPHERAL))
	MGC_ExitActiveUsbMode(toOtg, pPort);

    switch(toOtg)
    {
    case MUSB_AB_IDLE:
	/* no pending requests */
	pPort->bWantHost = FALSE;
	pPort->bWantSession = FALSE;
	pPort->pfProgramBusState(pPort);
	break;

    case MUSB_B_SRP_INIT:
	pPort->bWantSession = TRUE;
	pPort->pfProgramBusState(pPort);
#if 0
	pServices->pfArmTimer(pServices->pPrivateData, 0, MUSB_TB_SRP_FAIL * 1000,
	    FALSE, MGC_BSrpFailed);
#endif
	break;

    case MUSB_A_WAIT_BCON:
	pPort->bWantSession = TRUE;
	pPort->pfProgramBusState(pPort);
	pServices->pfArmTimer(pServices->pPrivateData, 0, MUSB_TS_SESSREQ * 100, /* * 1000*/
	    FALSE, MGC_WaitSessReqTimeout);
	break;

    case MUSB_A_PERIPHERAL:
    {
        MGC_Controller* pController = pPort->pController;
        uint8_t* pBase = (uint8_t*)pController->pControllerAddressIst;
        uint8_t power = MGC_Read8(pBase, MGC_O_HDRC_POWER);

        power |= MGC_M_POWER_SOFTCONN;

        MGC_Write8(pBase, MGC_O_HDRC_POWER, power);
    }
    case MUSB_B_PERIPHERAL:
	/* nothing to do; just prevent falling to default */
	break;

    case MUSB_B_WAIT_ACON:
	pServices->pfArmTimer(pServices->pPrivateData, 0, 200,
	    FALSE, MGC_BAase0BrstTimeout);
	break;

	/*
	*  must ignore a CONNECT if in A_SUSPEND; otherwise, schedule
	*  the start of the USB Reset with a slight delay.
	*/

    case MUSB_A_HOST:
	/* here is A-device only */
	if (pPort->bOtgState != MUSB_A_SUSPEND)
	{
	    pServices->pfArmTimer(pServices->pPrivateData, 0, 10,
		FALSE, MGC_HostResetStart);
	}
	break;

	/*
	*  For B-Device, DRC has already started the RESET, we need to activate a
	*  timer to terminate the USB Reset.
	*/
    case MUSB_B_HOST:
	/* turn off HostReq bit */
	info("\n###B-HOST: turn off HostReq bit and stop reset signal, bOtgState=0x%x\n", pPort->bOtgState);
	pPort->bWantHost = FALSE;
	pPort->bIsHnpAllowed = FALSE;
	pPort->pfProgramBusState(pPort);
	pServices->pfArmTimer(pServices->pPrivateData, 0, 100,
	    FALSE, MGC_HostResetComplete);
	break;

    case MUSB_A_SUSPEND:
	if(pPort->pRootDevice)
	{
	    pOtgDesc = (MUSB_OtgDescriptor*)MGC_FindDescriptor(
		(uint8_t*)pPort->pRootDevice->pCurrentConfiguration, 
		MUSB_SWAP16P((uint8_t*)&(pPort->pRootDevice->pCurrentConfiguration->wTotalLength)), 
		MUSB_DT_OTG, 0);
	    if(pOtgDesc && (pOtgDesc->bmAttributes & MUSB_OTG_HNP))
	    {
		MGC_SetHnpEnableIrp.pDevice = pPort->pRootDevice;
		MGC_SetHnpEnableIrp.pCompleteParam = pPort;
		MUSB_StartControlTransfer(pPort->pInterfacePort, &MGC_SetHnpEnableIrp);
	    }
		else
		{
			MUSB_SuspendBus((MUSB_BusHandle)pPort);
		}
	}
	break;

    default:
	MGC_DIAG1(1, pPort->pController, "Error changing OTG state to ", toOtg, 16, 0);
	return; 
    }   /*  end switch on toOTG */

    MGC_DIAG2(3, pPort->pController, "DrcChangeOtgState:from ", 
	pPort->bOtgState, " to ", toOtg, 10, 0);
    /* make permanent the state change */
    pPort->bOtgState = toOtg;

    MGC_CompleteOtgTransition(pPort);

}   /* MGC_DrcChangeOtgState */

/*
*  Notify applications
*  and programs the DRC to allow SUSPEND if we're a peripheral.
*/
void MGC_CompleteOtgTransition(MGC_Port* pPort)
{
    MUSB_OtgClient* pOtgClient = pPort->pOtgClient;

    if (MGC_AB_MODE(pPort->bOtgState) == MGC_AB_PERIPHERAL)
    {
	MGC_StartPeripheralMode(pPort);
    }

    if(pOtgClient)
    {
	pOtgClient->pfOtgState(pOtgClient->pPrivateData, (MUSB_BusHandle)pPort,
	    (uint8_t)pPort->bOtgState);
    }

}

/** Timer callback to turn off resume */
static void MGC_DrcCompleteResume(void* pParam, uint16_t wTimer)
{
    uint16_t wCount;
    uint16_t wIndex;
    MUSB_Device* pDevice;
    MGC_Device* pImplDevice;
    MUSB_DeviceDriver* pDriver;
    MGC_Controller* pController = (MGC_Controller*)pParam;
    MGC_Port* pPort = pController->pPort;

    pPort->bWantResume = FALSE;
    pPort->pfProgramBusState(pPort);

    /* inform drivers */
    wCount = MUSB_ListLength(&(pPort->ConnectedDeviceList));
    for(wIndex = 0; wIndex < wCount; wIndex++)
    {
	pDevice = (MUSB_Device*)MUSB_ListFindItem(&(pPort->ConnectedDeviceList), wIndex);
	if(pDevice)
	{
	    pImplDevice = pDevice->pPrivateData;
	    if(pImplDevice)
	    {
		pDriver = pImplDevice->pDriver;
		if(pDriver->pfBusSuspended)
		{
		    pDriver->pfBusResumed(pDriver->pPrivateData,
			(MUSB_BusHandle)pPort);
		}
	    }
	}
    }
}

/*
* Interrupt Service Routine to record USB "global" interrupts.
* Since these do not happen often and signify things of paramount importance, 
* it seems OK to check them individually
*/

uint8_t MGC_DrcUsbIsr(MGC_Port* pPort, uint8_t bIntrUsbVal)
{
    MGC_BsrItem qItem;
    uint32_t dwEndCount;
    uint32_t dwEndIndex;
    MGC_EndpointResource* pEnd;
    MUSB_IsochIrp* pIsochIrp;
    uint8_t bResult = 0;
    MUSB_SystemServices* pServices = pPort->pController->pSystemServices;
    uint8_t* pBase = (uint8_t*)pPort->pController->pControllerAddressIst;
    uint8_t bQueue;

    if(bIntrUsbVal)
    {
	if(bIntrUsbVal & MGC_M_INTR_RESUME)
	{
		info("MGC_M_INTR_RESUME bOtgState=0x%x\n", pPort->bOtgState);
		MGC_SetPowerSave(pPort, FALSE);
	    bResult++;
	    qItem.bCause = MGC_BSR_CAUSE_RESUME;
	    pServices->pfQueueBackgroundItem(pServices->pPrivateData, &qItem);
	}
	if(bIntrUsbVal & MGC_M_INTR_SESSREQ)
	{
	    info("\nMGC_M_INTR_SESSREQ bOtgState=0x%x\n", pPort->bOtgState);
	    bResult++;
	    qItem.bCause = MGC_BSR_CAUSE_SESSION_REQ;
	    pServices->pfQueueBackgroundItem(pServices->pPrivateData, &qItem);
	}
	if(bIntrUsbVal & MGC_M_INTR_VBUSERROR)
	{
	    info("MGC_M_INTR_VBUSERROR bOtgState=0x%x\n", pPort->bOtgState);
	    bResult++;
	    qItem.bCause = MGC_BSR_CAUSE_VBUS_ERROR;
	    pServices->pfQueueBackgroundItem(pServices->pPrivateData, &qItem);
	}
	if(bIntrUsbVal & MGC_M_INTR_SUSPEND)
	{
	    info("MGC_M_INTR_SUSPEND bOtgState=0x%x\n", pPort->bOtgState);
	    bResult++;
	    qItem.bCause = MGC_BSR_CAUSE_SUSPEND;
	    pServices->pfQueueBackgroundItem(pServices->pPrivateData, &qItem);
	}
	if(bIntrUsbVal & MGC_M_INTR_CONNECT)
	{
	    info("MGC_M_INTR_CONNECT(host only), bOtgState=0x%x\n", pPort->bOtgState);
	    pPort->bDisconnect = 0;
	    pPort->bIsHost = TRUE;
	    bResult++;
	    qItem.bCause = MGC_BSR_CAUSE_CONNECT;
	    pServices->pfQueueBackgroundItem(pServices->pPrivateData, &qItem);
	}
	if(bIntrUsbVal & MGC_M_INTR_DISCONNECT)
	{
	    info("MGC_M_INTR_DISCONNECT bOtgState=0x%x\n", pPort->bOtgState);
	    pPort->bDisconnect = 1;
	    bQueue = pPort->pfServiceDefaultEnd(pPort, &qItem);
	    if(bQueue)
	    {
		pServices->pfQueueBackgroundItem(pServices->pPrivateData, &qItem);
	    }
	    /* only handle disconnect if we are not resetting */
	    if(!pPort->bWantReset)
	    {
		MGC_DrcFlushAll(pPort);
		MGC_FunctionChangeState(pPort, MUSB_DEFAULT);
		bResult++;
		qItem.bCause = MGC_BSR_CAUSE_DISCONNECT;
		pServices->pfQueueBackgroundItem(pServices->pPrivateData, &qItem);
	    }
	}
	if(bIntrUsbVal & MGC_M_INTR_RESET)
	{
	    info("MGC_M_INTR_RESET bOtgState=0x%x\n", pPort->bOtgState);
#ifdef MUSB_STATS
	    pPort->dwResetCount++;
#endif
	    /* update bus state */
	    pPort->pfReadBusState(pPort);
	    if(pPort->bIsHost)
	    {
		/* host: safe to process in the background */
		bResult++;
		qItem.bCause = MGC_BSR_CAUSE_RESET;
		pServices->pfQueueBackgroundItem(pServices->pPrivateData, &qItem);
	    }
	    else
	    {
		/* peripheral: process now because next interrupt could be soon */
		/* update our state per hardware */
		pPort->bWantSession = pPort->bIsSession;
		pPort->bWantHost = FALSE;
		pPort->bFuncAddr = 0;
		pPort->wSetupDataIndex = 0;
		pPort->wSetupDataSize = 0;
		MGC_DrcFlushAll(pPort);
		MGC_FunctionChangeState(pPort, MUSB_DEFAULT);
		MGC_FunctionSpeedSet(pPort);
		switch(pPort->bOtgState)
		{
		case MUSB_AB_IDLE:
		    MGC_OtgStateGetId(pPort, TRUE);
		    break;
		case MUSB_B_SRP_INIT:
		    MGC_DrcChangeOtgState(MUSB_B_PERIPHERAL, pPort);
		    break;
		}
	    }
	}
	if(bIntrUsbVal & MGC_M_INTR_SOF)
	{
	    info("MGC_M_INTR_SOF bOtgState=0x%x\n", pPort->bOtgState);
	    /* update frame number */
	    pPort->pfReadBusState(pPort);

	    /* start any waiting isochronous transmits whose wait has expired */
	    dwEndCount = MUSB_ArrayLength(&(pPort->LocalEnds));
	    for(dwEndIndex = 1; dwEndIndex < dwEndCount; dwEndIndex++)
	    {
		pEnd = (MGC_EndpointResource*)MUSB_ArrayFetch(
		    &(pPort->LocalEnds), dwEndIndex);
		if(pEnd && (MUSB_ENDPOINT_XFER_ISOC == pEnd->bTrafficType) && 
		    pEnd->dwWaitFrameCount)
		{
		    pIsochIrp = (MUSB_IsochIrp*)pEnd->pTxIrp;
		    if(0 == --pEnd->dwWaitFrameCount)
		    {
			pPort->pfProgramStartTransmit(pPort, pEnd, 
			    pIsochIrp->pBuffer, pIsochIrp->adwLength[0], pIsochIrp);
		    }
		}
	    }

	    /* if host, prepare any required transfers */
	    if(pPort->bIsHost)
	    {
		MGC_RunScheduledTransfers(pPort);
	    }
	}

    }
    return bResult;
}

/*
* Finish DRC interrupt processing
*/

void MGC_DrcBsr(void* pParam)
{
#ifdef MUSB_OVERHEAD
    uint32_t dwTime;
    MGC_OverheadStats* pOverheadStats;
#endif
    MGC_BsrItem item;
    uint8_t bOk;
    uint8_t bTrafficType;
    MGC_EndpointResource* pLocalEnd;
    MUSB_Irp* pIrp;
    MUSB_ControlIrp* pControlIrp;
    MUSB_IsochIrp* pIsochIrp;
    MUSB_Port* pIfacePort = (MUSB_Port*)pParam;
    MGC_Port* pPort = (MGC_Port*)pIfacePort->pPrivateData;
    MGC_Controller* pController = (MGC_Controller*)pPort->pController;
    MUSB_SystemServices* pServices = pController->pSystemServices;
    MUSB_OtgClient* pOtgClient = (MUSB_OtgClient*)pPort->pOtgClient;

    bOk = pServices->pfDequeueBackgroundItem(pServices->pPrivateData, &item);
    while(bOk)
    {
	switch(item.bCause)
	{
	case MGC_BSR_CAUSE_RESUME:
	    /* resume */
#ifdef MUSB_STATS
	    pPort->dwResumeCount++;
#endif
	    MGC_DIAG(2, pController, "Resume Interrupt");
	    if(pPort->pFunctionClient)
	    {
		pPort->pFunctionClient->pfUsbState(pPort->pFunctionClient->pPrivateData,
		    (MUSB_BusHandle)pPort, pPort->bUsbState);
	    }
	    switch (pPort->bOtgState)
	    {
	    case MUSB_B_WAIT_ACON:
		/* OTG 6.8.2.4 */
		MGC_DrcChangeOtgState(MUSB_B_PERIPHERAL, pPort);
		break;

	    case MUSB_A_SUSPEND:
		/* OTG 6.8.1.5 */
		MGC_DrcChangeOtgState(MUSB_A_HOST, pPort);
		/* continue driving resume for 20 ms */
		pPort->bWantResume = TRUE;
		pPort->pfProgramBusState(pPort);
		pController->pSystemServices->pfArmTimer(
		    pController->pSystemServices->pPrivateData, 0, 20, FALSE,
		    MGC_DrcCompleteResume);
		break;
	    }
	    break;

	case MGC_BSR_CAUSE_SESSION_REQ:
		MGC_DIAG(3, pController, "session request Interrupt");
		if (!pPort->bSendSrp)
		{
			info("SRP opening the VBUS: bOtgState=0x%x\n", pPort->bOtgState);
			MGC_Controller* pController = pPort->pController;
			uint8_t* pBase = (uint8_t*)pController->pControllerAddressIst;
			uint8_t nDevCtl = MGC_Read8(pBase, MGC_O_HDRC_DEVCTL);
			nDevCtl |= MGC_M_DEVCTL_SESSION;

			MGC_Write8(pBase, MGC_O_HDRC_DEVCTL, nDevCtl);
		}
		/* qualify validity of SRP */
	    if(MUSB_AB_IDLE == pPort->bOtgState)
	    {
		/* OTG 6.8.1.1 */
		MGC_DrcChangeOtgState(MUSB_A_WAIT_BCON, pPort);
		pPort->bWantSession = TRUE;
		pPort->pfProgramBusState(pPort);
	    }
	    break;

	case MGC_BSR_CAUSE_VBUS_ERROR:
	    /* only occurs to A-Dev */
	    MGC_DIAG(2, pController, "Vbus Error Interrupt");
	    if(pPort->bVbusErrorRetries)
	    {
		pPort->bVbusErrorRetries--;
		MGC_DIAG(2, pController, "Retrying due to Vbus error");
		MGC_DrcChangeOtgState(MUSB_AB_IDLE, pPort);
		MGC_OtgStateGetId(pPort, FALSE);
	    }
	    else
	    {
		if(pOtgClient) pOtgClient->pfOtgError(pOtgClient->pPrivateData,
			pOtgClient, MUSB_STATUS_OTG_VBUS_INVALID);
		MGC_DrcChangeOtgState(MUSB_AB_IDLE, pPort);
	    }
	    break;

	case MGC_BSR_CAUSE_SUSPEND:
#ifdef MUSB_STATS
	    pPort->dwSuspendCount++;
#endif
	    MGC_DIAG(3, pController, "Suspend Interrupt");
	    /*info("Suspend Interrupt background, bOtgState 0x%x\n", pPort->bOtgState);*/

	    /* inform function client; things may change further, but that's OK */
	    if(pPort->pFunctionClient)
	    {
		pPort->pFunctionClient->pfUsbState(pPort->pFunctionClient->pPrivateData,
		    (MUSB_BusHandle)pPort, MUSB_SUSPENDED);
	    }
	    switch(pPort->bOtgState)
	    {
	    case MUSB_B_PERIPHERAL:
		pPort->bIsSuspend = TRUE;
		if (!pPort->bIsHnpAllowed) {
		    info("\nMUSB_B_PERIPHERAL: MUSB_SuspendBus!!!\n");
		    MUSB_SuspendBus((MUSB_BusHandle)pPort);
		}
		if(pPort->bIsHost)
		{
		    MGC_DrcChangeOtgState(MUSB_B_WAIT_ACON, pPort);
		}
		break;

	    case MUSB_A_HOST:
		pPort->bIsSuspend = TRUE;
		MGC_DrcChangeOtgState(MUSB_A_SUSPEND, pPort);
		break;

	    case MUSB_A_PERIPHERAL:
		MGC_DrcChangeOtgState(MUSB_A_WAIT_BCON, pPort);
	    }
	    break;

	case MGC_BSR_CAUSE_CONNECT:
	    MGC_DIAG(3, pController, "Connect Interrupt background");
		if (!pPort->bSendSrp)
			info("SRP open VBUS success: SRP test passed, bOtgState=0x%x\n\n", pPort->bOtgState);
	    pPort->pfReadBusState(pPort);

	    pPort->pRootDevice = NULL;
	    if((MUSB_B_WAIT_ACON == pPort->bOtgState) || 
		(MUSB_B_PERIPHERAL == pPort->bOtgState))
	    {
		MGC_DrcChangeOtgState(MUSB_B_HOST, pPort);
	    }
	    else
	    {
		MGC_DrcChangeOtgState(MUSB_A_HOST, pPort);
	    }
	    break;

	case MGC_BSR_CAUSE_DISCONNECT:
	    MGC_DIAG(3, pController, "Disconnect Interrupt background");
	    /* unequivocally clear this state */
	    pPort->bIsSuspend = FALSE;

	    /* clean up host state */
	    MGC_HostDestroy(pPort);

	    switch(pPort->bOtgState)
	    {
	    case MUSB_A_HOST:
		MUSB_DeviceDisconnected(pPort->pRootDevice);
		MGC_DrcChangeOtgState(MUSB_A_WAIT_BCON, pPort);
		break;

	    case MUSB_A_SUSPEND:
		if(pPort->bRelinquish && (MUSB_STATUS_OK == MGC_SetHnpEnableIrp.dwStatus))
		{
		    info("\nA-PERIPHERAL:in disconnect interupt background pullup softconn, change to MUSB_A_PERIPHERAL\n");
		    MUSB_DeviceDisconnected(pPort->pRootDevice);
		    /* OTG 6.8.1.5 */
		    MGC_DrcChangeOtgState(MUSB_A_PERIPHERAL, pPort);
		}
		else
		{
		    MGC_DrcChangeOtgState(MUSB_A_WAIT_BCON, pPort);
		}
		break;

	    case MUSB_B_HOST:
		MUSB_DeviceDisconnected(pPort->pRootDevice);
		MGC_DrcChangeOtgState(MUSB_B_PERIPHERAL, pPort);
		break;
	    }     
	    pPort->pRootDevice = NULL;
	    break;

	case MGC_BSR_CAUSE_RESET:
	    MGC_DIAG(2, pController, "Reset Interrupt background");
	    pPort->pfReadBusState(pPort);

	    if(!pPort->bIsSession)  
	    {
		/* session is blown */
		if(pPort->bBabbleRetries)
		{
		    pPort->bBabbleRetries--;
		    MGC_DIAG(2, pController, "Retrying due to babble");
		    MGC_DrcChangeOtgState(MUSB_AB_IDLE, pPort);
		    MGC_OtgStateGetId(pPort, FALSE);
		}
		else
		{
		    MGC_DIAG(1, pController, "Stopping due to babble");
		    pPort->pfResetPort(pPort);
		    pPort->pfProgramBusState(pPort);
		    MGC_DrcChangeOtgState(MUSB_AB_IDLE, pPort);
		}
	    }
	    break;

	case MGC_BSR_CAUSE_IRP_COMPLETE:
	case MGC_BSR_CAUSE_RX_IRP_COMPLETE:
	    pLocalEnd = (MGC_EndpointResource*)MUSB_ArrayFetch(
		&(pPort->LocalEnds), item.bLocalEnd);
#ifdef MUSB_OVERHEAD
	    pOverheadStats = MUSB_ArrayFetch(&(pPort->OverheadStats), item.bLocalEnd);
	    if(pOverheadStats)
	    {
		dwTime = pController->pUtils->pfGetTime() - item.dwTime;
		if(dwTime < pOverheadStats->dwOverheadMin)
		{
		    pOverheadStats->dwOverheadMin = dwTime;
		}
		if(dwTime > pOverheadStats->dwOverheadMax)
		{
		    pOverheadStats->dwOverheadMax = dwTime;
		}
	    }
#endif
	    if(pLocalEnd)
	    {
		bTrafficType = (MGC_BSR_CAUSE_IRP_COMPLETE == item.bCause) ?
		    pLocalEnd->bTrafficType : pLocalEnd->bRxTrafficType;
		switch(bTrafficType)
		{
		case MUSB_ENDPOINT_XFER_BULK:
		case MUSB_ENDPOINT_XFER_INT:
		    pIrp = (MUSB_Irp*)item.pData;
		    if(pIrp && pIrp->pfIrpComplete)
		    {
			pIrp->pfIrpComplete(pIrp->pCompleteParam, pIrp);
		    }
		    break;

		case MUSB_ENDPOINT_XFER_CONTROL:
		    pControlIrp = (MUSB_ControlIrp*)item.pData;
		    if(pControlIrp && pControlIrp->pfIrpComplete)
		    {
			pControlIrp->pfIrpComplete(pControlIrp->pCompleteParam,
			    pControlIrp);
		    }
		    break;

		case MUSB_ENDPOINT_XFER_ISOC:
		    pIsochIrp = (MUSB_IsochIrp*)item.pData;
		    if(pIsochIrp && pIsochIrp->pfIrpComplete)
		    {
			pIsochIrp->pfIrpComplete(pIsochIrp->pCompleteParam,
			    pIsochIrp);
		    }
		    break;
		}
		/* start next IRP if not started already */
		if(MGC_BSR_CAUSE_IRP_COMPLETE == item.bCause)
		{
		    if(!pLocalEnd->pTxIrp && !pLocalEnd->bStopTx)
		    {
			MGC_StartNextIrp(pPort, pLocalEnd, TRUE);
		    }
		}
		else
		{
		    if((!pLocalEnd->pRxIrp || (MUSB_ENDPOINT_XFER_INT == pLocalEnd->bRxTrafficType)) && 
			!pLocalEnd->bIsRxHalted)
		    {
			MGC_StartNextIrp(pPort, pLocalEnd, FALSE);
		    }
		}
	    }
	}

	/* get next item to process */
	bOk = pServices->pfDequeueBackgroundItem(pServices->pPrivateData, &item);
    }

}

/*
 * Service the default endpoint as host.
 * Returns: TRUE if more packets are required for this transaction
 */
uint8_t MGC_DrcServiceHostDefault(MGC_Port* pPort, uint16_t wCount, 
				  uint8_t bStatus, uint8_t* pbCsrVal)
{
    uint8_t bMore = FALSE;
    uint8_t* pFifoDest = NULL;
    uint16_t wFifoCount = 0;
    MGC_EndpointResource* pEnd = MUSB_ArrayFetch(&(pPort->LocalEnds), 0);
    MUSB_ControlIrp* pIrp = (MUSB_ControlIrp*)pEnd->pTxIrp;
    MUSB_DeviceRequest* pRequest = (MUSB_DeviceRequest*)pIrp->pOutBuffer;

    /* any error aborts */
    if(bStatus)
    {
	pIrp->dwStatus = bStatus;
	return FALSE;
    }

    if(!pEnd->bIsTx)
    {
	/* we are receiving from peripheral */
	pFifoDest = pIrp->pInBuffer + pIrp->dwActualInLength;
	wFifoCount = (uint16_t)MUSB_MIN(wCount, pIrp->dwInLength - pIrp->dwActualInLength);
#ifdef MUSB_DMA
	if(pEnd->pDmaChannel && pPort->pDmaController->pfDmaProgramChannel(
	    pEnd->pDmaChannel, pEnd->wPacketSize, 
	    pEnd->pDmaChannel->bDesiredMode, pFifoDest, 
	    MGC_MIN(wFifoCount, pEnd->pDmaChannel->dwMaxLength)))
	{
	    /* prevent caller from touching CSR0 */
	    *pbCsrVal = 0;
	    return TRUE;
	}
#endif
	/* unload FIFO */
	pPort->pfUnloadFifo(pPort, 0, wFifoCount, pFifoDest);
	pIrp->dwActualInLength += wFifoCount;
	if((pIrp->dwActualInLength < pIrp->dwInLength) && (wCount == pEnd->wPacketSize))
	{
	    bMore = TRUE;
	}
    }
    else
    {
	/* we are sending to peripheral */
	pIrp->dwActualOutLength += pEnd->dwTxSize;
	pEnd->dwTxSize = 0;
	if(pRequest->bmRequestType & MUSB_DIR_IN)
	{
	    /* this means we just did setup; switch to IN */
	    pEnd->bIsTx = FALSE;
#ifdef MUSB_DMA
	    if(pEnd->pDmaChannel)
	    {
		pPort->pDmaController->pfDmaReleaseChannel(pEnd->pDmaChannel);
		pEnd->pDmaChannel = pPort->pDmaController->pfDmaAllocateChannel(
		    pPort->pDmaController->pPrivateData, 0, FALSE, 
		    MUSB_ENDPOINT_XFER_CONTROL, pEnd->wPacketSize);
		if(pEnd->pDmaChannel)
		{
		    pEnd->pDmaChannel->dwActualLength = 0L;
		    if(!pPort->pDmaController->pfDmaProgramChannel(pEnd->pDmaChannel, 
			pEnd->wPacketSize, pEnd->pDmaChannel->bDesiredMode, 
			pIrp->pInBuffer, 
			MGC_MIN(pIrp->dwInLength, pEnd->pDmaChannel->dwMaxLength)))
		    {
			pPort->pDmaController->pfDmaReleaseChannel(pEnd->pDmaChannel);
			pEnd->pDmaChannel = NULL;
		    }
		}
	    }
#endif
	    bMore = TRUE;
	}
	else
	{
	    pFifoDest = (uint8_t*)(pIrp->pOutBuffer + pIrp->dwActualOutLength);
	    wFifoCount = (uint16_t)MUSB_MIN(pEnd->wPacketSize, 
		pIrp->dwOutLength - pIrp->dwActualOutLength);
#ifdef MUSB_DMA
	    if(pEnd->pDmaChannel && pPort->pDmaController->pfDmaProgramChannel(
		pEnd->pDmaChannel, pEnd->wPacketSize, 
		pEnd->pDmaChannel->bDesiredMode, pFifoDest, 
		MGC_MIN(wFifoCount, pEnd->pDmaChannel->dwMaxLength)))
	    {
		/* prevent caller from touching CSR0 */
		*pbCsrVal = 0;
		return TRUE;
	    }
#endif
	    /* load FIFO and signal need to transmit */
	    if(wFifoCount)
	    {
		pPort->pfLoadFifo(pPort, 0, wFifoCount, pFifoDest);
		pEnd->dwTxSize = wFifoCount;
		bMore = TRUE;
	    }
	}
    }

    if(bMore)
    {
	*pbCsrVal = pEnd->bIsTx ? MGC_M_CSR0_TXPKTRDY : MGC_M_CSR0_H_REQPKT;
    }
    else
    {
	*pbCsrVal = 0;
    }
    return bMore;
}

/*
 * Check if device is acceptable; take appropriate action if not.
 * The enumerator calls this immediately BEFORE configuring a peripheral.
 * Returns: TRUE if device is acceptable; FALSE if not (and appropriate action taken)
 */
uint8_t MGC_DrcAcceptDevice(MGC_Port* pPort, MUSB_Device* pDevice,
			    MUSB_DeviceDriver* pDriver)
{
    const MUSB_OtgDescriptor* pOtgDesc;
    uint8_t bOk = TRUE;
    MUSB_OtgClient* pOtgClient = pPort->pOtgClient;

    /* handle test device */
    if((6666 == pDevice->DeviceDescriptor.idVendor) && 
	(0xbadd == pDevice->DeviceDescriptor.idProduct))
    {
	/* inform client */
	if(pOtgClient) pOtgClient->pfOtgError(pOtgClient->pPrivateData, pPort,
	    MUSB_STATUS_UNSUPPORTED_DEVICE);
	bOk = FALSE;
    }

    /* inform client in "hubs not supported" situation */
    if(!pPort->bIsMultipoint && (MUSB_CLASS_HUB == pDevice->DeviceDescriptor.bDeviceClass))
    {
	if(pOtgClient) pOtgClient->pfOtgError(pOtgClient->pPrivateData, pPort,
	    MUSB_STATUS_UNSUPPORTED_HUB);
	bOk = FALSE;
    }

    /* inform client if no driver found and not the above situations */
    if(bOk && !pDriver)
    {
	if(pOtgClient) pOtgClient->pfOtgError(pOtgClient->pPrivateData, pPort,
	    MUSB_STATUS_UNSUPPORTED_DEVICE);
	bOk = FALSE;
    }

    if(bOk)
    {
	/* if we are A-device, set a_hnp_support if device supports HNP */
	if(!pPort->bConnectorId && pPort->pRootDevice)
	{
	    pOtgDesc = (MUSB_OtgDescriptor*)MGC_FindDescriptor(
		(uint8_t*)pPort->pRootDevice->pCurrentConfiguration, 
		MUSB_SWAP16P((uint8_t*)&(pPort->pRootDevice->pCurrentConfiguration->wTotalLength)), 
		MUSB_DT_OTG, 0);
	    if(pOtgDesc && (pOtgDesc->bmAttributes & MUSB_OTG_HNP))
	    {
		MGC_SetHnpSupportIrp.pDevice = pPort->pRootDevice;
		MGC_SetHnpSupportIrp.pCompleteParam = pPort;
		MUSB_StartControlTransfer(pPort->pInterfacePort, &MGC_SetHnpSupportIrp);
	    }
	}
    }
    else
    {
	MUSB_RelinquishHost((MUSB_BusHandle)pPort);
    }

    return bOk;
}

void MGC_DrcFinishResume(MGC_Port* pPort)
{
	uint32_t dwTimeout = 10;

	pPort->pfReadBusState(pPort);
	if(pPort->bIsHost)
	{
		/* continue driving resume for 20 ms */
		dwTimeout = 20;
	}
	pPort->pController->pSystemServices->pfArmTimer(
		pPort->pController->pSystemServices->pPrivateData, 0, dwTimeout, FALSE,
		MGC_DrcCompleteResume);
}

/*
 * Set host power capability, but ignore if we are B-device
 */
uint32_t MGC_DrcSetHostPower(MGC_Controller* pController, 
							 uint16_t wPower)
{
	uint32_t result = MUSB_STATUS_INVALID_ARGUMENT;
    MGC_Port* pPort = pController->pPort;

	pPort->wHostMaxPower = wPower;
    pPort->pfReadBusState(pPort);

	if(pPort->bConnectorId)
	{
		result = MUSB_STATUS_OK;
	}
	else
	{
		result = MGC_HostSetMaxPower(pPort, wPower);
	}
	return result;
}

/**
 * Enter the OTG state GetId
 */
static void MGC_OtgStateGetId(MGC_Port* pPort, uint8_t bIsReset)
{
    pPort->bWantSuspend = FALSE;
    pPort->bWantSession = TRUE;
    pPort->pfProgramBusState(pPort);
    
    /*
    * Per UTMI+, the PHY has 50 ms to read the connector ID.
    * For proper OTG state transition, one would expect we should wait 50 ms here.
    * However, the core already does this, and if we wait, we will fall behind
	* processing the interrupts from the core.
    */

    pPort->pfReadBusState(pPort);

    if(!pPort->bIsHost)
    {
	if(pPort->bConnectorId)
	{
	    if(bIsReset)
	    {
		MGC_DrcChangeOtgState(MUSB_B_PERIPHERAL, pPort);
	    }
	    else
	    {
		MGC_DrcChangeOtgState(MUSB_B_SRP_INIT, pPort);
	    }
	}
	else
	{
	    /* as A-device, we always support HNP */
	    pPort->bIsHnpSupported = TRUE;
	    MGC_DrcChangeOtgState(MUSB_A_WAIT_BCON, pPort);
	}
    }
}

/*
* Implementation
*/
MUSB_BusHandle MUSB_RegisterOtgClient(MUSB_Port* pPort, 
				      MUSB_FunctionClient* pFunctionClient,
				      MUSB_HostClient* pHostClient,
				      MUSB_OtgClient* pOtgClient)
{
    MUSB_OtgDescriptor* pOtgDesc;
    uint32_t dwStatus = 0;
    void* pResult = NULL;
    MGC_Port* pImplPort = (MGC_Port*)pPort->pPrivateData;

    /* session still active? */
    if(!pImplPort->bSessionActive)
    {
	/* host client just has callbacks */
	pImplPort->pHostClient = pHostClient;

	if(pFunctionClient)
	{
	    /* function client requires verification */
	    dwStatus = MGC_FunctionRegisterClient(pImplPort, pFunctionClient);
	}
	if(0 == dwStatus)
	{
	    /* OK; scan for OTG descriptor and set flag for HNP capability */
	    pImplPort->bIsHnpSupported = FALSE;
	    if(pFunctionClient)
	    {
		pOtgDesc = (MUSB_OtgDescriptor*)MGC_FindDescriptor(
		    pFunctionClient->pStandardDescriptors,
		    pFunctionClient->wDescriptorBufferLength, MUSB_DT_OTG, 0);
	    }
	    if(pOtgDesc)
	    {
		pImplPort->bIsHnpSupported = (pOtgDesc->bmAttributes & MUSB_OTG_HNP) ? TRUE : FALSE;
	    }

	    pImplPort->bVbusErrorRetries = MUSB_VBUS_ERROR_RETRIES;
	    pImplPort->bBabbleRetries = MUSB_BABBLE_RETRIES;

	    /* nothing active; reset port */
	    pImplPort->pfResetPort(pImplPort);

	    /* start */
	    pImplPort->pFunctionClient = pFunctionClient;
	    pImplPort->pOtgClient = pOtgClient;
	    pImplPort->bSessionActive = TRUE;

	    MGC_OtgStateGetId(pImplPort, FALSE);

	    /* set return value */
	    pResult = pImplPort;
	}
    }

    return pResult;
}

/*
 * For OTG, stop session and relinquish host role to another
 */
void MUSB_RelinquishHost(MUSB_BusHandle hBus)
{
    MGC_Port* pPort = (MGC_Port*)hBus;
    MGC_Controller* pController = (MGC_Controller*)pPort->pController;
    MUSB_SystemServices* pServices = pController->pSystemServices;
    info("\n\nMUSB_RelinquishHost: pPort->bConnectorId=%d\n", pPort->bConnectorId);
    pPort->bRelinquish = TRUE;
    if(!pPort->bConnectorId)
    {
	MGC_DrcChangeOtgState(MUSB_A_SUSPEND, pPort);
    }
    else
    {
	/* suspend bus */
	pPort->pfReadBusState(pPort);
	pPort->bWantSuspend = TRUE;
	pPort->pfProgramBusState(pPort);
	/* transition */
	MGC_DrcChangeOtgState(MUSB_B_PERIPHERAL, pPort);
	/* remove suspend later */
	pServices->pfArmTimer(pServices->pPrivateData, 0, 10, FALSE,
	    MGC_RemoveSuspend);
    }
}

uint8_t MUSB_RequestBus(MUSB_BusHandle hBus)
{
    MGC_Port* pPort = (MGC_Port*)hBus;

    switch(pPort->bOtgState)
    {
    case MUSB_AB_IDLE:
	MGC_OtgStateGetId(pPort, FALSE);
	return TRUE;
    }
    if(!pPort->bIsSession)
    {
	MGC_OtgStateGetId(pPort, FALSE);
	return TRUE;
    }
    return FALSE;
}

#if MUSB_DIAG > 0

/*
* Dump info relevant to the given pipe
*/
int MGC_DrcDumpPipe(MGC_Controller* pController, MGC_Pipe* pPipe)
{
    /* TODO */
    return 0;
}

#endif	/* MUSB_DIAG */

#else

/* satisfy some compilers */
uint8_t MGC_bNoDrc;

#endif	/* OTG */
