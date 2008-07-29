
#include "H264AVCCommonLib.h"

H264AVCCOMMONLIB_API UInt g_nSymbolCounter[16] = { 0,0,0,0,  0,0,0,0,  0,0,0,0,  0,0,0,0 };
H264AVCCOMMONLIB_API UInt g_nLayer = 0;


const ErrVal Err::m_nOK =                (0);
const ErrVal Err::m_nERR =               (-1);
const ErrVal Err::m_nEndOfStream =       (-2);
const ErrVal Err::m_nEndOfFile =         (-3);
const ErrVal Err::m_nEndOfBuffer =       (-4);
const ErrVal Err::m_nInvalidParameter =  (-5);
const ErrVal Err::m_nDataNotAvailable =  (-6);


H264AVC_NAMESPACE_BEGIN
H264AVC_NAMESPACE_END

