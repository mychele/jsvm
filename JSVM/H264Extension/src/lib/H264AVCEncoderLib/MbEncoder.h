
#if !defined(AFX_MBENCODER_H__F725C8AD_2589_44AD_B904_62FE2A7F7D8D__INCLUDED_)
#define AFX_MBENCODER_H__F725C8AD_2589_44AD_B904_62FE2A7F7D8D__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "DistortionIf.h"
#include "RateDistortionIf.h"
#include "MotionEstimation.h"
#include "MbTempData.h"
#include "MbCoder.h"
#include "BitCounter.h"
#include "UvlcWriter.h"
#include "H264AVCCommonLib/Quantizer.h"


H264AVC_NAMESPACE_BEGIN

class Transform;
class MbFGSCoefMap;
class IntraPredictionSearch;
class CodingParameter;

//TMM_WP
#define MAX_REF_FRAMES 64
//TMM_WP


class MbEncoder
: protected MbCoder
, public UvlcWriter
, protected BitCounter
{
protected:
	MbEncoder();
	virtual ~MbEncoder();

public:
  static ErrVal create( MbEncoder*& rpcMbEncoder );
  ErrVal destroy();

  ErrVal init(  Transform* pcTransform,
                IntraPredictionSearch* pcIntraPrediction,
                MotionEstimation *pcMotionEstimation,
                CodingParameter* pcCodingParameter,
                RateDistortionIf* pcRateDistortionIf,
                XDistortion* pcXDistortion
                );

  ErrVal uninit();
  ErrVal initSlice( const SliceHeader& rcSH);

  Void  setIPCMRate( UInt ui )  { m_uiIPCMRate = ui; }
  UInt  getIPCMRate()   const   { return m_uiIPCMRate; }

  IntMbTempData* getBestIntData() {return m_pcIntMbBestData; }

  ErrVal  encodeIntra         ( MbDataAccess&   rcMbDataAccess,
                                MbDataAccess*   pcMbDataAccessBase,
                                Frame*		pcOrgFrame,
                                Frame*       pcFrame,
                                Frame*       pcRecSubband,
                                Frame*       pcBaseLayer,
                                Frame*       pcPredSignal,
                                Double        dLambda,
                                Double&       rdCost );

  ErrVal  encodeResidual      ( MbDataAccess&   rcMbDataAccess,
                                Frame*       pcOrgFrame,
                                Frame*       pcFrame,
                                Frame*       pcResidual,
                                Frame*       pcBaseSubband,
                                Bool&           rbCoded,
                                Double          dLambda,
                                Int             iMaxDeltaQp );

  ErrVal  encodeInterP        ( MbDataAccess&   rcMbDataAccess,
                                MbDataAccess*   pcMbDataAccessBase,
                                Frame*       pcOrgFrame,
                                Frame*       pcFrame,
                                Frame*       pcRecSubband,
                                Frame*       pcPredSignal,
                                Frame*       pcBaseLayerRec,
                                Frame*       pcBaseLayerSbb,
                                RefFrameList&   rcRefFrameList0,
                                Bool            bMCBlks8x8Disable,
                                UInt            uiMaxNumMv,
                                Double          dLambda,
                                Double&         rdCost,
                                Bool            bSkipModeAllowed);

  ErrVal  compensatePrediction( MbDataAccess&   rcMbDataAccess,
                                Frame*       pcMCFrame,
                                RefFrameList&   rcRefFrameList0,
                                RefFrameList&   rcRefFrameList1,
                                Bool            bCalcMv,
                                Bool            bFaultTolerant);

  ErrVal  compensateUpdate(      MbDataAccess&   rcMbDataAccess,
                                 Frame*       pcMCFrame,
                                 Int             iRefIdx,
                                 ListIdx         eListPrd,
                                 Frame*       pcPrdFrame);


  ErrVal  estimatePrediction  ( MbDataAccess&   rcMbDataAccess,
                                MbDataAccess*   pcMbDataAccessBase,
                                RefFrameList&   rcRefFrameList0,
                                RefFrameList&   rcRefFrameList1,
                                const Frame* pcBaseLayerFrame,
                                const Frame* pcBaseLayerResidual,
                                const Frame& rcOrigFrame,
                                Frame&       rcIntraRecFrame,
                                UInt            uiMaxNumMv,
                                Bool            bMCBlks8x8Disable,
                                Bool            bBiPred8x8Disable,
                                UInt            uiNumMaxIter,
                                UInt            uiIterSearchRange,
                                Double          dLambda,
                                Double&         rdCost,
                                Bool            bSkipModeAllowed );
  ErrVal  encodeMacroblock    ( MbDataAccess&   rcMbDataAccess,
                                Frame*       pcFrame,
                                RefFrameList&   rcList0,
                                RefFrameList&   rcList1,
                                UInt            uiMaxNumMv,
                                Bool            bMCBlks8x8Disable,
                                Bool            bBiPred8x8Disable,
                                UInt            uiNumMaxIter,
                                UInt            uiIterSearchRange,
                                Double          dLambda );

//TMM_WP
  ErrVal getPredWeights( SliceHeader& rcSH, ListIdx eLstIdx,
                         Double(*pafWeight)[3], Frame* pOrgFrame,
                         RefFrameList& rcRefFrameListX);


  ErrVal getPredOffsets( SliceHeader& rcSH, ListIdx eLstIdx,
                         Double(*pafOffsets)[3], Frame* pOrgFrame,
                         RefFrameList& rcRefFrameListX);

//TMM_WP



  //JVT-R057 LA-RDO{
  Void setLARDOEnable( Bool bLARDO)  { m_bLARDOEnable= bLARDO; }

  //JVT-V079 Low-complexity MB mode decision
  Void setLowComplexMbEnable( Int iLayer, Bool bEnable )    { m_bLowComplexMbEnable[iLayer] = bEnable; }

  Void setLayerID (UInt uiLayer)     { m_uiLayerID=uiLayer;}

  Void setPLR( UInt auiPLR[5])       { for(UInt i=0;i<5;i++) m_auiPLR[i] = auiPLR[i];}

  Void setRatio( Double adRatio[5][2])
  {
	  for(UInt i=0;i<5;i++)
		  for(UInt j=0;j<2;j++)
			  m_aadRatio[i][j] = adRatio[i][j];
  }
 Void setMBSSD      ( UInt uiSSD)      { m_uiMBSSD=uiSSD; }

  Bool getLARDOEnable(){ return m_bLARDOEnable;}

 Void setFrameEcEp  ( Frame* p1)    { m_pcFrameEcEp=p1; }

  Int  GetEC_REC            ( YuvPicBuffer* pPic1,
                              YuvPicBuffer* pPic2,
                              Int              blockX,
                              Int              blockY);

  Void  getChannelDistortion( MbDataAccess&    rcMbDataAccess,
	                            Frame&           rcRefFrame,
	                            Int              *distortion,
	                            Int              iMvX,
	                            Int              iMvY,
	                            Int              startX,
	                            Int              startY,
	                            Int              blockX,
	                            Int              blockY,
	                            Bool             bSpatial=false);

  Int getEpRef() { return m_iEpRef; }

  Void setEpRef(Int iRef)   { m_iEpRef=iRef; }

  Void  getDistortion       (Int              iDList0,
                             Int              iDList1,
                             SampleWeighting* pcSampleWeighting,
                             MbDataAccess&    rcMbDataAccess);
  //JVT-R057 LA-RDO}

  //S051{
  Void		setUseBDir	( Bool bUse){ m_bUseBDir = bUse;}
  //S051}
  Void          setBaseLayerRec     ( Frame*   pcBaseLayerRec  )   { m_pcBaseLayerFrame    = pcBaseLayerRec;   }
  Frame*     getBaseLayerRec     ()  { return  m_pcBaseLayerFrame;     }

  Void  setBaseModeAllowedFlag( Bool b ) { m_bBaseModeAllowedFlag = b; }

protected:
  ErrVal  xScale4x4Block        ( TCoeff*            piCoeff,
                                  const UChar*       pucScale,
                                  UInt               uiStart,
                                  const QpParameter& rcQP );

  ErrVal  xScale8x8Block        ( TCoeff*            piCoeff,
                                  const UChar*       pucScale,
                                  const QpParameter& rcQP );
  ErrVal  xScaleTCoeffs         ( MbDataAccess&      rcMbDataAccess,
                                  MbTransformCoeffs& rcTCoeffs );

  ErrVal  xSetRdCostIntraMb     ( IntMbTempData&    rcMbTempData,
                                  UInt              uiCoeffBits,
                                  Bool              bBSlice,
                                  Bool              bBLSkip );

  ErrVal  xSetRdCostInterMb     ( IntMbTempData&    rcMbTempData,
                                  MbDataAccess*     pcMbDataAccessBase,
                                  RefFrameList&     rcRefFrameList0,
                                  RefFrameList&     rcRefFrameList1,
                                  Bool              bBLSkip          = false,
                                  UInt              uiAdditionalBits = 0,
                                  Bool              bSkipMCPrediction = false,
                                  Frame*         pcBaseLayerRec = 0,
                                  Bool              bLowComplexity=false // JVT-V079
                                  );
  ErrVal  xSetRdCost8x8InterMb  ( IntMbTempData&    rcMbTempData,
                                  MbDataAccess*     pcMbDataAccessBaseMotion,
                                  RefFrameList&     rcRefFrameList0,
                                  RefFrameList&     rcRefFrameList1,
                                  Bool              bBLSkip          = false,
                                  UInt              uiAdditionalBits = 0,
                                  Bool              bSkipMCPrediction = false,
                                  Frame*         pcBaseLayerRec = 0
                                  );
  ErrVal  xSetRdCostInterSubMb  ( IntMbTempData&    rcMbTempData,
                                  RefFrameList&     rcRefFrameList0,
                                  RefFrameList&     rcRefFrameList1,
                                  B8x8Idx           c8x8Idx,
                                  Bool              bTrafo8x8,
                                  UInt              uiAddBits,
                                  Bool              bLowComplexity=false // JVT-V079
                                );

  ErrVal  xEncodeChromaIntra        ( IntMbTempData& rcMbTempData, UInt& ruiExtCbp, UInt& ruiBits,
                                      Bool bLowComplexity=false );

  ErrVal  xEncode4x4IntraBlock      ( IntMbTempData& rcMbTempData, LumaIdx cIdx,     UInt& ruiBits, UInt& ruiExtCbp, UInt mpMode, UInt lambda_val, Bool LowComplex=false );
  ErrVal  xEncode4x4InterBlock      ( IntMbTempData&               rcMbTempData,
                                      LumaIdx                      cIdx,
                                      UInt&                        ruiBits,
                                      UInt&                        ruiExtCbp );
  ErrVal  xEncode8x8InterBlock      ( IntMbTempData&               rcMbTempData,
                                      B8x8Idx                      c8x8Idx,
                                      UInt&                        ruiBits,
                                      UInt&                        ruiExtCbp );
  ErrVal  xEncode8x8IntraBlock      ( IntMbTempData& rcMbTempData, B8x8Idx cIdx,     UInt& ruiBits, UInt& ruiExtCbp );
  ErrVal  xEncodeChromaTexture      ( IntMbTempData& rcMbTempData, UInt& ruiExtCbp, UInt& ruiBits, Bool bLowComplexity = false );

  Void    xReStoreParameter     ( MbDataAccess&     rcMbDataAccess, IntMbTempData& rcMbBestData );
  Void    xUpDateBest           ( IntMbTempData&    rcMbTempData );

  ErrVal  xCheckInterMbMode8x8  ( IntMbTempData*&   rpcMbTempData,
                                  IntMbTempData*&   rpcMbBestData,
                                  IntMbTempData*    pcMbRefData,
                                  RefFrameList&     rcRefFrameList0,
                                  RefFrameList&     rcRefFrameList1,
                                  Bool              bBLSkip,
                                  MbDataAccess*     pcMbDataAccessBaseMotion,
                                  Frame*         pcBaseLayerRec = 0
                                  );

  ErrVal  xEstimateMbIntraBL    ( MbDataAccess&  rcMbDataAccess,   // JVT-V079,
                                  IntMbTempData*&   rpcMbTempData,
                                  IntMbTempData*&   rpcMbBestData,
                                  const Frame*   pcBaseLayerRec,
                                  Bool              bBSlice,
                                  MbDataAccess*     pcMbDataAccessBase );
  ErrVal  xEstimateMbIntraBL8x8 ( IntMbTempData*&   rpcMbTempData,
                                  MbDataAccess*    pcMbDataAccessBase,
                                  IntMbTempData*&   rpcMbBestData,
                                  const Frame*   pcBaseLayerRec,
                                  Bool              bBSlice,
                                  Bool              bBLSkip );

  ErrVal  xEstimateMbIntra16    ( IntMbTempData*&   rpcMbTempData,
                                  IntMbTempData*&   rpcMbBestData,
                                  Bool              bBSlice,
                                  Bool              bBLSkip=false );
  ErrVal  xEstimateMbIntra8     ( IntMbTempData*&   rpcMbTempData,
                                  IntMbTempData*&   rpcMbBestData,
                                  Bool              bBSlice,
                                  Bool              bBLSkip=false );
  ErrVal  xEstimateMbIntra4     ( MbDataAccess&  rcMbDataAccess,   // JVT-V079
                                  IntMbTempData*&   rpcMbTempData,
                                  IntMbTempData*&   rpcMbBestData,
                                  Bool              bBSlice,
                                  Bool              bBLSkip=false );

  ErrVal  xEstimateMbPCM        ( IntMbTempData*&   rpcMbTempData,
                                  IntMbTempData*&   rpcMbBestData,
                                  Bool              bBSlice  );
  ErrVal  xEstimateMbPCMRewrite ( IntMbTempData*&   rpcMbTempData,
                                  IntMbTempData*&   rpcMbBestData );
  // JVT-W043 {
  UInt    jsvmCalcMAD           ( IntMbTempData*&   rpcMbBestData,
                                  MbDataAccess&  rcMbDataAccess );
  // JVT-W043 }

  ErrVal  xEstimateMbSkip       ( IntMbTempData*&   rpcMbTempData,
                                  IntMbTempData*&   rpcMbBestData,
                                  RefFrameList&     rcRefFrameList0,
                                  RefFrameList&     rcRefFrameList1 );
  ErrVal  xEstimateMbBLSkip     ( IntMbTempData*&   rpcIntMbTempData,
                                  IntMbTempData*&   rpcIntMbBestData,
                                  RefFrameList&     rcRefFrameList0,
                                  RefFrameList&     rcRefFrameList1,
                                  const Frame*   pcBaseLayerRec,
                                  UInt              uiMaxNumMv,
                                  Bool              bBiPred8x8Disable,
                                  Bool              bBSlice,
                                  MbDataAccess*     pcMbDataAccessBaseMotion,
                                  MbDataAccess&     rcMbDataAccess,
                                  Bool              bResidualPred );

  ErrVal  xEstimateMbDirect     ( IntMbTempData*&   rpcMbTempData,
                                  IntMbTempData*&   rpcMbBestData,
                                  RefFrameList&     rcRefFrameList0,
                                  RefFrameList&     rcRefFrameList1,
                                  MbDataAccess*     pcMbDataAccessBaseMotion,
                                  UInt              uiMaxNumMv,
                                  Bool              bResidualPred,
                                  UInt              Qp,
                                  Bool              bSkipModeAllowed=true);
  ErrVal  xEstimateMb16x16      ( IntMbTempData*&   rpcMbTempData,
                                  IntMbTempData*&   rpcMbBestData,
                                  RefFrameList&     rcRefFrameList0,
                                  RefFrameList&     rcRefFrameList1,
                                  UInt              uiMaxNumMv,
                                  UInt              uiNumMaxIter,
                                  UInt              uiIterSearchRange,
                                  Bool              bQPelRefinementOnly,
                                  MbDataAccess*     pcMbDataAccessBaseMotion,
                                  Bool              bResidualPred );
  ErrVal  xEstimateMb16x8       ( IntMbTempData*&   rpcMbTempData,
                                  IntMbTempData*&   rpcMbBestData,
                                  RefFrameList&     rcRefFrameList0,
                                  RefFrameList&     rcRefFrameList1,
                                  UInt              uiMaxNumMv,
                                  UInt              uiNumMaxIter,
                                  UInt              uiIterSearchRange,
                                  Bool              bQPelRefinementOnly,
                                  MbDataAccess*     pcMbDataAccessBaseMotion,
                                  Bool              bResidualPred );
  ErrVal  xEstimateMb8x16       ( IntMbTempData*&   rpcMbTempData,
                                  IntMbTempData*&   rpcMbBestData,
                                  RefFrameList&     rcRefFrameList0,
                                  RefFrameList&     rcRefFrameList1,
                                  UInt              uiMaxNumMv,
                                  UInt              uiNumMaxIter,
                                  UInt              uiIterSearchRange,
                                  Bool              bQPelRefinementOnly,
                                  MbDataAccess*     pcMbDataAccessBaseMotion,
                                  Bool              bResidualPred );
  ErrVal  xEstimateMb8x8        ( IntMbTempData*&   rpcMbTempData,
                                  IntMbTempData*&   rpcMbBestData,
                                  RefFrameList&     rcRefFrameList0,
                                  RefFrameList&     rcRefFrameList1,
                                  UInt              uiMaxNumMv,
                                  Bool              bMCBlks8x8Disable,
                                  Bool              bBiPred8x8Disable,
                                  UInt              uiNumMaxIter,
                                  UInt              uiIterSearchRange,
                                  Bool              bQPelRefinementOnly,
                                  MbDataAccess*     pcMbDataAccessBaseMotion,
                                  Bool              bResidualPred );
  ErrVal  xEstimateMb8x8Frext   ( IntMbTempData*&   rpcMbTempData,
                                  IntMbTempData*&   rpcMbBestData,
                                  RefFrameList&     rcRefFrameList0,
                                  RefFrameList&     rcRefFrameList1,
                                  UInt              uiMaxNumMv,
                                  UInt              uiNumMaxIter,
                                  UInt              uiIterSearchRange,
                                  Bool              bQPelRefinementOnly,
                                  MbDataAccess*     pcMbDataAccessBaseMotion,
                                  Bool              bResidualPred );
  ErrVal  xEstimateSubMbDirect  ( Par8x8            ePar8x8,
                                  IntMbTempData*&   rpcMbTempData,
                                  IntMbTempData*&   rpcMbBestData,
                                  RefFrameList&     rcRefFrameList0,
                                  RefFrameList&     rcRefFrameList1,
                                  UInt              uiMaxNumMv,
                                  Bool              bTrafo8x8,
                                  UInt              uiAddBits );
  ErrVal  xEstimateSubMb8x8     ( Par8x8            ePar8x8,
                                  IntMbTempData*&   rpcMbTempData,
                                  IntMbTempData*&   rpcMbBestData,
                                  RefFrameList&     rcRefFrameList0,
                                  RefFrameList&     rcRefFrameList1,
                                  UInt              uiMaxNumMv,
                                  Bool              bTrafo8x8,
                                  UInt              uiNumMaxIter,
                                  UInt              uiIterSearchRange,
                                  UInt              uiAddBits,
                                  Bool              bQPelRefinementOnly,
                                  MbDataAccess*     pcMbDataAccessBaseMotion );
  ErrVal  xEstimateSubMb8x4     ( Par8x8            ePar8x8,
                                  IntMbTempData*&   rpcMbTempData,
                                  IntMbTempData*&   rpcMbBestData,
                                  RefFrameList&     rcRefFrameList0,
                                  RefFrameList&     rcRefFrameList1,
                                  UInt              uiMaxNumMv,
                                  Bool              bBiPred8x8Disable,
                                  UInt              uiNumMaxIter,
                                  UInt              uiIterSearchRange,
                                  UInt              uiAddBits,
                                  Bool              bQPelRefinementOnly,
                                  MbDataAccess*     pcMbDataAccessBaseMotion );
  ErrVal  xEstimateSubMb4x8     ( Par8x8            ePar8x8,
                                  IntMbTempData*&   rpcMbTempData,
                                  IntMbTempData*&   rpcMbBestData,
                                  RefFrameList&     rcRefFrameList0,
                                  RefFrameList&     rcRefFrameList1,
                                  UInt              uiMaxNumMv,
                                  Bool              bBiPred8x8Disable,
                                  UInt              uiNumMaxIter,
                                  UInt              uiIterSearchRange,
                                  UInt              uiAddBits,
                                  Bool              bQPelRefinementOnly,
                                  MbDataAccess*     pcMbDataAccessBaseMotion );
  ErrVal  xEstimateSubMb4x4     ( Par8x8            ePar8x8,
                                  IntMbTempData*&   rpcMbTempData,
                                  IntMbTempData*&   rpcMbBestData,
                                  RefFrameList&     rcRefFrameList0,
                                  RefFrameList&     rcRefFrameList1,
                                  UInt              uiMaxNumMv,
                                  Bool              bBiPred8x8Disable,
                                  UInt              uiNumMaxIter,
                                  UInt              uiIterSearchRange,
                                  UInt              uiAddBits,
                                  Bool              bQPelRefinementOnly,
                                  MbDataAccess*     pcMbDataAccessBaseMotion );

  ErrVal  xCheckBestEstimation  ( IntMbTempData*&   rpcMbTempData,
                                  IntMbTempData*&   rpcMbBestData );
  ErrVal  xStoreEstimation      ( MbDataAccess&     rcMbDataAccess,
                                  IntMbTempData&    rcMbBestData,
                                  Frame*            pcRecSubband,
                                  Frame*            pcPredSignal,
                                  RefFrameList*     pcRefList0,
                                  RefFrameList*     pcRefList1,
                                  Bool              bMotionFieldEstimation,
                                  YuvMbBuffer*      pcBaseLayerBuffer );
  Bool    xCheckUpdate          ( YuvMbBuffer&   rcPredBuffer,
                                  YuvMbBuffer&   rcOrigBuffer,
                                  LumaIdx           cIdx,
                                  Int               iXSize,
                                  Int               iYSize );


  ErrVal  xEncode16x16ResidualMB( IntMbTempData&    rcMbTempData,
                                  UInt&             ruiBits,
                                  UInt&             ruiExtCbp );


  UInt  xCalcMbCbp    ( UInt uiExtCbp );

  Void   reCalcBlock4x4               ( IntMbTempData& rcMbTempData, LumaIdx c4x4Idx );
  Void   reCalcBlock8x8               ( IntMbTempData& rcMbTempData, B8x8Idx c8x8Idx, Int mode );
  Void   reCalcChroma                 ( IntMbTempData& rcMbTempData );

  ErrVal reCalcBlock4x4Rewrite        ( IntMbTempData& rcMbTempData, LumaIdx c4x4Idx );
  ErrVal reCalcBlock8x8Rewrite        ( IntMbTempData& rcMbTempData, B8x8Idx c8x8Idx, Int mode );
  ErrVal reCalcBlock16x16Rewrite      ( IntMbTempData& rcMbTempData );
  ErrVal reCalcChromaRewrite          ( IntMbTempData& rcMbTempData );

  ErrVal xCheckSkipSliceMb            ( IntMbTempData& rcMbTempData );
  ErrVal xCheckSkipSliceMbIntra4      ( IntMbTempData& rcMbTempData, LumaIdx c4x4Idx, UInt& ruiAbsSum );
  ErrVal xCheckSkipSliceMbIntra8      ( IntMbTempData& rcMbTempData, B8x8Idx c8x8Idx, UInt& ruiAbsSum );
  ErrVal xCheckSkipSliceMbIntra16     ( IntMbTempData& rcMbTempData, UInt&   ruiAcAbs );
  ErrVal xCheckSkipSliceMbIntraChroma ( IntMbTempData& rcMbTempData, UInt&   ruiChromaCbp );

  ErrVal xAdjustRewriteReconstruction ( IntMbTempData& rcMbTempData );

private:
  UChar xGetFrameBits ( ListIdx eLstIdx, Int iRefPic );

protected:
  CodingParameter* m_pcCodingParameter;
  Transform*   m_pcTransform;
  IntraPredictionSearch*    m_pcIntraPrediction;
  MotionEstimation *m_pcMotionEstimation;
  RateDistortionIf* m_pcRateDistortionIf;
  XDistortion*  m_pcXDistortion;
  Bool bInitDone;
  Bool m_bISlice;
  Bool m_bBSlice;
  Bool m_bCabac;
  IntMbTempData  m_acIntMbTempData[5];
  IntMbTempData* m_pcIntMbBestData;
  IntMbTempData* m_pcIntMbTempData;
  IntMbTempData* m_pcIntMbBest8x8Data;
  IntMbTempData* m_pcIntMbTemp8x8Data;
  IntMbTempData* m_pcIntMbBestIntraChroma;

  YuvMbBuffer  *m_pcIntOrgMbPelData;
  YuvPicBuffer *m_pcIntPicBuffer;
  YuvPicBuffer *m_pcIntraPredPicBuffer;

  UInt m_uiMaxRefFrames[2];
  UInt m_uiMaxRefPics[2];

  BitWriteBufferIf* m_BitCounter;

  //JVT-V079 Low-complexity MB mode decision
  Bool m_bLowComplexMbEnable[MAX_LAYERS];

  //JVT-R057 LA-RDO{
  Bool m_bLARDOEnable;
  UInt m_uiLayerID;
  UInt m_auiPLR[5];
  Double m_aadRatio[5][2];
  UInt m_uiMBSSD;
  Frame* m_pcFrameEcEp;
  Frame* m_pcBaseLayerFrame;
  Int  m_iEpRef;
  Double m_dWr0;
  Double m_dWr1;
  //JVT-R057 LA-RDO}

  //S051{
  Bool		m_bUseBDir;
  //S051}
  Bool  m_bBaseModeAllowedFlag;
  UInt  m_uiIPCMRate;
};


H264AVC_NAMESPACE_END

#endif // !defined(AFX_MBENCODER_H__F725C8AD_2589_44AD_B904_62FE2A7F7D8D__INCLUDED_)
