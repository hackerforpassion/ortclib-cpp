/*

 Copyright (c) 2014, Hookflash Inc. / Hookflash Inc.
 All rights reserved.

 Redistribution and use in source and binary forms, with or without
 modification, are permitted provided that the following conditions are met:

 1. Redistributions of source code must retain the above copyright notice, this
 list of conditions and the following disclaimer.
 2. Redistributions in binary form must reproduce the above copyright notice,
 this list of conditions and the following disclaimer in the documentation
 and/or other materials provided with the distribution.

 THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR
 ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

 The views and conclusions contained in the software and documentation are those
 of the authors and should not be interpreted as representing official policies,
 either expressed or implied, of the FreeBSD Project.
 
 */

#pragma once

#include <ortc/internal/types.h>

#include <ortc/IICETypes.h>

namespace ortc
{
  namespace internal
  {
    //-------------------------------------------------------------------------
    //-------------------------------------------------------------------------
    //-------------------------------------------------------------------------
    //-------------------------------------------------------------------------
    #pragma mark
    #pragma mark RTPPacket
    #pragma mark

    class RTPPacket
    {
    protected:
      struct make_private {};

    public:
      struct VideoOrientationHeaderExtension;
      struct VideoOrientation6HeaderExtension;

    public:
      //-----------------------------------------------------------------------
      #pragma mark
      #pragma mark RTPPacket::Extension
      #pragma mark

      struct HeaderExtension
      {
        // see https://tools.ietf.org/html/rfc5285

        BYTE mID {};

        const BYTE *mData {};
        size_t mDataSizeInBytes {};
        size_t mPostPaddingSize {};

        HeaderExtension *mNext {};
      };

      //-----------------------------------------------------------------------
      #pragma mark
      #pragma mark RTPPacket::ClientToMixerExtension
      #pragma mark

      struct ClientToMixerExtension : public HeaderExtension
      {
        // see https://tools.ietf.org/html/rfc6464

        ClientToMixerExtension(const HeaderExtension &header);
        ClientToMixerExtension(
                               BYTE id,
                               bool voiceActivity,
                               BYTE level
                               );

        bool voiceActivity() const;
        BYTE level() const;

      public:
        BYTE mLevelBuffer {};
      };

      //-----------------------------------------------------------------------
      #pragma mark
      #pragma mark RTPPacket::MixerToClientExtension
      #pragma mark

      struct MixerToClientExtension : public HeaderExtension
      {
        // see https://tools.ietf.org/html/rfc6464

        static const size_t kMaxLevelCount {0xF};

        MixerToClientExtension(const HeaderExtension &header);
        MixerToClientExtension(
                               BYTE id,
                               BYTE *levels,
                               size_t count
                               );

        size_t levelsCount() const;

        BYTE unusedBit(size_t index) const;
        BYTE level(size_t index) const;

      public:
        BYTE mLevelBuffer[kMaxLevelCount] {};
      };
      
      //-----------------------------------------------------------------------
      #pragma mark
      #pragma mark RTPPacket::MixerToClientExtension
      #pragma mark

      struct MidHeadExtension : public HeaderExtension
      {
        // https://tools.ietf.org/html/draft-ietf-mmusic-sdp-bundle-negotiation-23#section-14.3

        static const size_t kMaxMidLength {0xFF};

        MidHeadExtension(const HeaderExtension &header);
        MidHeadExtension(
                         BYTE id,
                         const char *mid
                         );

        const char *mid() const;

        ElementPtr toDebug() const;

      public:
        BYTE mMidBuffer[kMaxMidLength+sizeof(char)] {};
      };
      
      //-----------------------------------------------------------------------
      #pragma mark
      #pragma mark RTPPacket::MixerToClientExtension
      #pragma mark

      struct ExtendedSourceInformationHeaderExtension : public HeaderExtension
      {
        // urn:example:params:rtp-hdrext:extended-ssrc-info

        struct RTXType {
          static const BYTE kType {1};
        };
        struct FECType {
          static const BYTE kType {2};
        };


        ExtendedSourceInformationHeaderExtension(const HeaderExtension &header);
        ExtendedSourceInformationHeaderExtension(
                                                 const RTXType &,
                                                 BYTE id,
                                                 DWORD associatedSSRC
                                                 );
        ExtendedSourceInformationHeaderExtension(
                                                 const FECType &,
                                                 BYTE id,
                                                 bool associatedSSRCIsValid,
                                                 DWORD associatedSSRC
                                                 );


        BYTE type() const                   {return mType;}

        bool isAssociatedSSRCValid() const  {return mIsAssociateSSRCValid;}
        DWORD associatedSSRC() const        {return mAssociatedSSRC;}

        ElementPtr toDebug() const;

      public:
        BYTE mType {0};
        bool mIsAssociateSSRCValid {};
        DWORD mAssociatedSSRC {};

        BYTE mEncoded[sizeof(DWORD)*2] {};
      };

      //-----------------------------------------------------------------------
      #pragma mark
      #pragma mark RTPPacket::VideoOrientationHeaderExtension
      #pragma mark

      struct VideoOrientationHeaderExtension : public HeaderExtension
      {
        struct Clockwise {};
        struct CounterClockwise {};

        VideoOrientationHeaderExtension(const VideoOrientation6HeaderExtension &);
        VideoOrientationHeaderExtension(const HeaderExtension &header);
        VideoOrientationHeaderExtension(
                                        const Clockwise &,
                                        bool frontFacingCamera, // true = front facing, false = backfacing
                                        bool flip, // horizontal left-right flip (mirro)
                                        UINT orientation
                                        );
        VideoOrientationHeaderExtension(
                                        const CounterClockwise &,
                                        bool frontFacingCamera, // true = front facing, false = backfacing
                                        bool flip, // horizontal left-right flip (mirro)
                                        UINT orientation
                                        );

        bool frontFacing() const;
        bool backFacing() const;
        bool flip() const;
        UINT degreesClockwise() const;
        UINT degreesCounterClockwise() const;

        ElementPtr toDebug() const;

      public:
        BYTE mEncoded[1] {};
      };

      //-----------------------------------------------------------------------
      #pragma mark
      #pragma mark RTPPacket::VideoOrientationHeaderExtension
      #pragma mark

      struct VideoOrientation6HeaderExtension : public VideoOrientationHeaderExtension
      {
        VideoOrientation6HeaderExtension(const HeaderExtension &header);
        VideoOrientation6HeaderExtension(
                                         const Clockwise &,
                                         bool frontFacingCamera, // true = front facing, false = backfacing
                                         bool flip, // horizontal left-right flip (mirro)
                                         UINT orientation
                                         );
        VideoOrientation6HeaderExtension(
                                         const CounterClockwise &,
                                         bool frontFacingCamera, // true = front facing, false = backfacing
                                         bool flip, // horizontal left-right flip (mirro)
                                         UINT orientation
                                         );

        UINT degreesClockwise() const;
        UINT degreesCounterClockwise() const;

        ElementPtr toDebug() const;

      public:
      };

    public:
      //-----------------------------------------------------------------------
      #pragma mark
      #pragma mark (public)
      #pragma mark

      RTPPacket(const make_private &);
      ~RTPPacket();

      static RTPPacketPtr create(const BYTE *buffer, size_t bufferLengthInBytes);
      static RTPPacketPtr create(const SecureByteBlock &buffer);
      static RTPPacketPtr create(SecureByteBlockPtr buffer);  // NOTE: ownership of buffer is taken

      const BYTE *ptr() const;
      size_t size() const;
      SecureByteBlockPtr buffer() const;

      BYTE version() const {return mVersion;}
      size_t padding() const {return mPadding;}
      size_t cc() const {return static_cast<size_t>(mCC);}
      bool m() const {return mM;}
      BYTE pt() const {return mPT;}
      WORD sequenceNumber() const {return mSequenceNumber;}
      DWORD timestamp() const {return mTimestamp;}
      DWORD ssrc() const {return mSSRC;}

      size_t headerSize() {return mHeaderSize;}
      size_t headerExtensionSize() {return mHeaderExtensionSize;}
      size_t payloadSize() {return mPayloadSize;}

      DWORD getCSRC(size_t index) const;
      const BYTE *payload() const;
      size_t payloadSize() const {return mPayloadSize;}

      size_t totalHeaderExtensions() const {return mTotalHeaderExtensions;}
      HeaderExtension *firstHeaderExtension() const {return mHeaderExtensions;}
      HeaderExtension *getHeaderExtensionAtIndex(size_t index) const;
      BYTE headerExtensionAppBits() const {return mHeaderExtensionAppBits;}

      size_t headerExtensionPrepaddedSize() const {return mHeaderExtensionPrepaddedSize;}
      const BYTE *headerExtensionParseStopped() const {return mHeaderExtensionParseStoppedPos;}
      size_t headerExtensionParseStoppedSize() const {return mHeaderExtensionParseStoppedSize;}

      void changeHeaderExtensions(HeaderExtension *firstExtension);

      ElementPtr toDebug() const;

    protected:
      //-----------------------------------------------------------------------
      #pragma mark
      #pragma mark (internal)
      #pragma mark

      static Log::Params slog(const char *message);
      Log::Params log(const char *message) const;
      Log::Params debug(const char *message) const;

      bool parse();

    public:
      SecureByteBlockPtr mBuffer;

      BYTE mVersion {};
      size_t mPadding {};
      BYTE mCC {};
      bool mM {};
      BYTE mPT;
      WORD mSequenceNumber {};
      DWORD mTimestamp {};
      DWORD mSSRC {};

      size_t mHeaderSize {};
      size_t mHeaderExtensionSize {};
      size_t mPayloadSize;

      size_t mTotalHeaderExtensions {};
      HeaderExtension *mHeaderExtensions {};
      BYTE mHeaderExtensionAppBits {};

      size_t mHeaderExtensionPrepaddedSize {};
      const BYTE *mHeaderExtensionParseStoppedPos {};
      size_t mHeaderExtensionParseStoppedSize {};
    };

  }
}

