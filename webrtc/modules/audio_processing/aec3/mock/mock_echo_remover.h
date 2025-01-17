///*
// *  Copyright (c) 2017 The WebRTC project authors. All Rights Reserved.
// *
// *  Use of this source code is governed by a BSD-style license
// *  that can be found in the LICENSE file in the root of the source
// *  tree. An additional intellectual property rights grant can be found
// *  in the file PATENTS.  All contributing project authors may
// *  be found in the AUTHORS file in the root of the source tree.
// */

//#ifndef MODULES_AUDIO_PROCESSING_AEC3_MOCK_MOCK_ECHO_REMOVER_H_
//#define MODULES_AUDIO_PROCESSING_AEC3_MOCK_MOCK_ECHO_REMOVER_H_

//#include <vector>

//#include "absl/types/optional.h"
//#include "modules/audio_processing/aec3/echo_path_variability.h"
//#include "modules/audio_processing/aec3/echo_remover.h"
//#include "modules/audio_processing/aec3/render_buffer.h"
//#include "test/gmock.h"

//namespace webrtc {
//namespace test {

//class MockEchoRemover : public EchoRemover {
// public:
//  MockEchoRemover();
//  virtual ~MockEchoRemover();

//  MOCK_METHOD6(ProcessCapture,
//               void(EchoPathVariability echo_path_variability,
//                    bool capture_signal_saturation,
//                    const absl::optional<DelayEstimate>& delay_estimate,
//                    RenderBuffer* render_buffer,
//                    std::vector<std::vector<std::vector<float>>>* linear_output,
//                    std::vector<std::vector<std::vector<float>>>* capture));
//  MOCK_CONST_METHOD0(Delay, absl::optional<int>());
//  MOCK_METHOD1(UpdateEchoLeakageStatus, void(bool leakage_detected));
//  MOCK_CONST_METHOD1(GetMetrics, void(EchoControl::Metrics* metrics));
//};

//}  // namespace test
//}  // namespace webrtc

//#endif  // MODULES_AUDIO_PROCESSING_AEC3_MOCK_MOCK_ECHO_REMOVER_H_
