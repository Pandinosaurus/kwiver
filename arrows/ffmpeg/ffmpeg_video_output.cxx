// This file is part of KWIVER, and is distributed under the
// OSI-approved BSD 3-Clause License. See top-level LICENSE file or
// https://github.com/Kitware/kwiver/blob/master/LICENSE for details.

/// \file
/// \brief Implementation of FFmpeg video writer.

#include "arrows/ffmpeg/ffmpeg_init.h"
#include "arrows/ffmpeg/ffmpeg_video_output.h"
#include "arrows/ffmpeg/ffmpeg_video_settings.h"

extern "C" {
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libavutil/imgutils.h>
#include <libswscale/swscale.h>
}

namespace kv = kwiver::vital;

namespace kwiver {

namespace arrows {

namespace ffmpeg {

// ----------------------------------------------------------------------------
class ffmpeg_video_output::impl
{
public:
  impl();

  ~impl();

  bool write_next_packet();

  void write_remaining_packets();

private:
  friend class ffmpeg_video_output;

  AVFormatContext* format_context;
  AVOutputFormat* output_format;
  AVStream* video_stream;
  AVStream* metadata_stream;
  AVCodecContext* codec_context;
  AVCodec* codec;
  SwsContext* image_conversion_context;

  kv::logger_handle_t logger;

  size_t frame_count;
  AVRational frame_rate;
};

// ----------------------------------------------------------------------------
ffmpeg_video_output::impl
::impl()
  : format_context{ nullptr },
    output_format{ nullptr },
    video_stream{ nullptr },
    metadata_stream{ nullptr },
    codec_context{ nullptr },
    codec{ nullptr },
    image_conversion_context{ nullptr },
    logger{},
    frame_count{ 0 },
    frame_rate{ 0, 1 }
{
  ffmpeg_init();
}

// ----------------------------------------------------------------------------
ffmpeg_video_output::impl::~impl() {}

// ----------------------------------------------------------------------------
bool
ffmpeg_video_output::impl
::write_next_packet()
{
  AVPacket packet = {};
  av_init_packet( &packet );

  // Attempt to read next encoded packet
  auto const success =
    avcodec_receive_packet( codec_context, &packet );

  if( success == AVERROR( EAGAIN ) || success == AVERROR_EOF )
  {
    // Failed expectedly: no packet to read
    av_packet_unref( &packet );
    return false;
  }
  if( success < 0 )
  {
    // Failed unexpectedly
    av_packet_unref( &packet );
    VITAL_THROW( kv::video_runtime_exception, "Failed to receive packet" );
  }

  // Succeeded; write to file
  if( av_interleaved_write_frame( format_context, &packet ) < 0 )
  {
    VITAL_THROW( kv::video_runtime_exception, "Failed to write packet" );
  }

  return true;
}

// ----------------------------------------------------------------------------
void
ffmpeg_video_output::impl
::write_remaining_packets()
{
  // Enter "draining mode" - i.e. signal end of file
  avcodec_send_frame( codec_context, nullptr );

  while( write_next_packet() ) {}
}

// ----------------------------------------------------------------------------
ffmpeg_video_output
::ffmpeg_video_output() : d{ new impl{} }
{
  attach_logger( "ffmpeg_video_output" );
  d->logger = logger();

  set_capability( kv::algo::video_output::SUPPORTS_FRAME_RATE, true );
  set_capability( kv::algo::video_output::SUPPORTS_FRAME_TIME, true );
  set_capability( kv::algo::video_output::SUPPORTS_METADATA, true );
}

// ----------------------------------------------------------------------------
ffmpeg_video_output::~ffmpeg_video_output()
{
  close();
}

// ----------------------------------------------------------------------------
vital::config_block_sptr
ffmpeg_video_output
::get_configuration() const
{
  // TODO
  auto config = vital::algorithm::get_configuration();
  return config;
}

// ----------------------------------------------------------------------------
void
ffmpeg_video_output
::set_configuration( kv::config_block_sptr config )
{
  // TODO
  auto existing_config = vital::algorithm::get_configuration();
  existing_config->merge_config( config );
}

// ----------------------------------------------------------------------------
bool
ffmpeg_video_output
::check_configuration( kv::config_block_sptr config ) const
{
  // TODO
  return true;
}

// ----------------------------------------------------------------------------
void
ffmpeg_video_output
::open(
  std::string video_name, vital::video_settings const* generic_settings )
{
  // Ensure we start from a blank slate
  close();

  auto const settings =
    generic_settings
    ? dynamic_cast< ffmpeg_video_settings const* >( generic_settings )
    : nullptr;
  if( !settings )
  {
    VITAL_THROW( kv::invalid_value,
                 "Must provide ffmpeg_video_settings to open()" );
  }

  d->output_format = av_guess_format( nullptr, video_name.c_str(), nullptr );

  // Allocate output format context
  avformat_alloc_output_context2(
    &d->format_context, d->output_format, nullptr, nullptr );
  if( !d->format_context )
  {
    LOG_DEBUG( logger(),
               "Could not deduce output format for filename `"
               << video_name << "`; defaulting to MPEG" );
    avformat_alloc_output_context2(
      &d->format_context, nullptr, "mpeg", nullptr );
  }
  if( !d->format_context )
  {
    VITAL_THROW( kv::video_runtime_exception,
                 "Failed to allocate format context" );
  }

  // Configure video codec
  d->codec = avcodec_find_encoder( d->output_format->video_codec );
  if( !d->codec )
  {
    VITAL_THROW( kv::video_runtime_exception, "Failed to find codec" );
  }

  // Find best pixel format
  auto pixel_format = static_cast< AVPixelFormat >( -1 );
  for( auto ptr = d->codec->pix_fmts; ptr && *ptr != -1; ++ptr )
  {
    if( *ptr == settings->pixel_format )
    {
      pixel_format = settings->pixel_format;
      break;
    }
  }
  if( pixel_format == -1 )
  {
    pixel_format =
      avcodec_find_best_pix_fmt_of_list(
        d->codec->pix_fmts, AV_PIX_FMT_RGB24, false, nullptr );
  }

  // Create and configure codec context
  d->codec_context = avcodec_alloc_context3( d->codec );
  if( !d->codec_context )
  {
    VITAL_THROW( kv::video_runtime_exception,
                 "Failed to allocate codec context" );
  }
  d->codec_context->time_base = av_inv_q( settings->frame_rate );
  d->codec_context->pix_fmt = pixel_format;
  d->codec_context->width = settings->width;
  d->codec_context->height = settings->height;
  d->codec_context->framerate = settings->frame_rate;
  d->codec_context->sample_aspect_ratio = settings->sample_aspect_ratio;
  if( d->codec_context->bit_rate > 0 )
  {
    d->codec_context->bit_rate = settings->bit_rate;
    d->codec_context->bit_rate_tolerance = settings->bit_rate_tolerance;
  }
  if( d->codec->id == settings->codec_id )
  {
    d->codec_context->gop_size = settings->gop_size;
    d->codec_context->level = settings->level;
    d->codec_context->profile = settings->profile;
  }

  // Create video stream
  if( d->output_format->video_codec == AV_CODEC_ID_NONE )
  {
    VITAL_THROW( kv::invalid_value,
                 std::string{} + "Specified format `" +
                 d->output_format->long_name + "` does not support video" );
  }

  d->video_stream = avformat_new_stream( d->format_context, d->codec );
  if( !d->video_stream )
  {
    VITAL_THROW( kv::video_runtime_exception,
                 "Failed to allocate video stream" );
  }
  d->video_stream->time_base = d->codec_context->time_base;
  d->video_stream->codecpar->codec_type = AVMEDIA_TYPE_VIDEO;
  d->video_stream->codecpar->codec_id = d->output_format->video_codec;
  d->video_stream->codecpar->width = d->codec_context->width;
  d->video_stream->codecpar->height = d->codec_context->height;
  d->video_stream->codecpar->format = d->codec_context->pix_fmt;

  if( avcodec_open2( d->codec_context, d->codec, nullptr ) < 0 )
  {
    VITAL_THROW( kv::video_runtime_exception, "Failed to open codec" );
  }

  av_dump_format(
    d->format_context, d->video_stream->index, video_name.c_str(), 1 );

  // Open streams
  if( avio_open( &d->format_context->pb,
                 video_name.c_str(), AVIO_FLAG_WRITE ) < 0 )
  {
    VITAL_THROW( kv::file_write_exception, video_name,
                 std::string{} + "Failed to open video file for writing" );
  }

  auto const output_status =
    avformat_init_output( d->format_context, nullptr );
  if( output_status == AVSTREAM_INIT_IN_WRITE_HEADER )
  {
    if( avformat_write_header( d->format_context, nullptr ) < 0 )
    {
      VITAL_THROW( kv::video_runtime_exception,
                   "Failed to write video header" );
    }
  }
  if( output_status != AVSTREAM_INIT_IN_INIT_OUTPUT )
  {
    VITAL_THROW( kv::video_runtime_exception,
                 "Failed to initialize output stream" );
  }

  d->frame_rate = settings->frame_rate;
}

// ----------------------------------------------------------------------------
void
ffmpeg_video_output
::close()
{
  d->codec = nullptr;

  if( d->format_context )
  {
    d->write_remaining_packets();

    // Write closing bytes of video format
    if( av_write_trailer( d->format_context ) < 0 )
    {
      VITAL_THROW( kv::video_runtime_exception, "Failed to write trailer" );
    }

    // Close video file
    if( d->format_context->pb )
    {
      if( avio_closep( &d->format_context->pb ) < 0 )
      {
        VITAL_THROW( kv::video_runtime_exception,
                     "Failed to close video file" );
      }
    }

    // Destroy video context
    avcodec_free_context( &d->codec_context );

    // Destroy video encoder
    avformat_free_context( d->format_context );
    d->format_context = nullptr;
  }

  // Destroy image converter
  if( d->image_conversion_context )
  {
    sws_freeContext( d->image_conversion_context );
    d->image_conversion_context = nullptr;
  }

  d->frame_count = 0;
  d->frame_rate = { 0, 1 };
}

// ----------------------------------------------------------------------------
bool
ffmpeg_video_output
::good() const
{
  return d->format_context;
}

// ----------------------------------------------------------------------------
void
ffmpeg_video_output
::add_image( kv::image_container_sptr const& image,
             VITAL_UNUSED kv::timestamp const& ts )
{
  // These structs ensure no memory leaks even when an exception is thrown.
  // Could use std::unique_ptr, but the syntax is a bit less clean
  struct frame_autodeleter
  {
    ~frame_autodeleter() { av_frame_free( &ptr ); }

    AVFrame* ptr;
  };

  struct image_autodeleter
  {
    ~image_autodeleter() { av_freep( &ptr->data[ 0 ] ); }

    AVFrame* ptr;
  };

  // Create frame object to represent incoming image
  auto const frame = av_frame_alloc();
  if( !frame )
  {
    VITAL_THROW( kv::video_runtime_exception, "Failed to allocate frame" );
  }

  frame_autodeleter const frame_deleter{ frame };

  // Fill in a few mandatory fields
  frame->width = image->width();
  frame->height = image->height();
  frame->format = AV_PIX_FMT_RGB24;

  // Allocate storage based on those fields
  if( av_frame_get_buffer( frame, 0 ) < 0 )
  {
    VITAL_THROW( kv::video_runtime_exception,
                 "Failed to allocate frame data" );
  }

  // Give the frame the raw pixel data
  auto const image_ptr =
    static_cast< uint8_t* >( image->get_image().memory()->data() );
  if( av_image_fill_arrays(
        frame->data, frame->linesize, image_ptr, AV_PIX_FMT_RGB24,
        image->width(), image->height(), 1 ) < 0 )
  {
    VITAL_THROW( kv::video_runtime_exception, "Failed to fill frame image" );
  }

  // Create frame object to hold the image after conversion to the required
  // pixel format
  auto const converted_frame = av_frame_alloc();
  if( !converted_frame )
  {
    VITAL_THROW( kv::video_runtime_exception, "Failed to allocate frame" );
  }

  frame_autodeleter const converted_frame_deleter{ converted_frame };

  // Fill in a few mandatory fields
  converted_frame->width = image->width();
  converted_frame->height = image->height();
  converted_frame->format = d->codec_context->pix_fmt;

  // Allocate storage based on those fields
  if( av_frame_get_buffer( converted_frame, 0 ) < 0 )
  {
    VITAL_THROW( kv::video_runtime_exception,
                 "Failed to allocate frame data" );
  }

  // Allocate a buffer to store the converted pixel data
  if( av_image_alloc(
        converted_frame->data, converted_frame->linesize,
        image->width(), image->height(), d->codec_context->pix_fmt, 1 ) < 0 )
  {
    VITAL_THROW( kv::video_runtime_exception,
                 "Failed to allocate frame image" );
  }

  image_autodeleter const converted_image_deleter{ converted_frame };

  // Specify which conversion to perform
  d->image_conversion_context = sws_getCachedContext(
    d->image_conversion_context,
    image->width(), image->height(), AV_PIX_FMT_RGB24,
    image->width(), image->height(), d->codec_context->pix_fmt,
    SWS_BICUBIC, nullptr, nullptr, nullptr );
  if( !d->image_conversion_context )
  {
    VITAL_THROW( kv::video_runtime_exception,
                 "Failed to create image conversion context" );
  }

  // Convert the pixel format
  if( sws_scale(
        d->image_conversion_context,
        frame->data, frame->linesize,
        0, image->height(),
        converted_frame->data, converted_frame->linesize ) !=
      static_cast< int >( image->height() ) )
  {
    VITAL_THROW( kv::video_runtime_exception,
                 "Failed to convert frame image pixel format" );
  }

  // Try to send image to video encoder
  converted_frame->pts =
    static_cast< int64_t >(
      static_cast< double >( d->frame_count ) *
      d->video_stream->time_base.den / d->video_stream->time_base.num /
      d->frame_rate.num * d->frame_rate.den + 0.5 );

  auto frame_success = avcodec_send_frame( d->codec_context, converted_frame );

  // If the video encoder's buffers are full, we have to process some output
  // packets before we can send a new image
  while( frame_success == AVERROR( EAGAIN ) )
  {
    if( !d->write_next_packet() )
    {
      throw std::logic_error{ "Frame provided after EOF" };
    }
    frame_success = avcodec_send_frame( d->codec_context, converted_frame );
  }

  if( frame_success < 0 )
  {
    VITAL_THROW( kv::video_runtime_exception,
                 "Failed to send frame to encoder" );
  }

  ++d->frame_count;
}

// ----------------------------------------------------------------------------
void
ffmpeg_video_output
::add_metadata( kwiver::vital::metadata_sptr const& md )
{
  // TODO
}

} // namespace ffmpeg

} // namespace arrows

} // namespace kwiver