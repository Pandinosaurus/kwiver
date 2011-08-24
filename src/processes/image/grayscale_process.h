/*ckwg +5
 * Copyright 2011 by Kitware, Inc. All Rights Reserved. Please refer to
 * KITWARE_LICENSE.TXT for licensing information, or contact General Counsel,
 * Kitware, Inc., 28 Corporate Drive, Clifton Park, NY 12065.
 */

#ifndef VISTK_PROCESSES_IMAGE_GRAYSCALE_PROCESS_H
#define VISTK_PROCESSES_IMAGE_GRAYSCALE_PROCESS_H

#include "image-config.h"

#include <vistk/pipeline/process.h>

namespace vistk
{

/**
 * \class grayscale_process
 *
 * \brief A process which converts an image to grayscale.
 *
 * \process A process for converting an image into grayscale.
 *
 * \iports
 *
 * \iport{rgbimage} An RGB image to convert to grayscale.
 *
 * \oports
 *
 * \oport{grayimage} The number generated for the step.
 *
 * \reqs
 *
 * \req The \port{rgbimage} and \port{grayimage} ports must be connected.
 */
template<class T>
class VISTK_PROCESSES_IMAGE_NO_EXPORT grayscale_process
  : public process
{
  public:
    /**
     * \brief Constructor.
     *
     * \param config Contains config for the process.
     */
    grayscale_process(config_t const& config);
    /**
     * \brief Destructor.
     */
    ~grayscale_process();
  protected:
    /**
     * \brief Turns an image into grayscale.
     */
    void _step();
  private:
    class priv;
    boost::shared_ptr<priv> d;
};

process_t VISTK_PROCESSES_IMAGE_NO_EXPORT create_grayscale_byte_process(config_t const& config);
process_t VISTK_PROCESSES_IMAGE_NO_EXPORT create_grayscale_float_process(config_t const& config);

}

#endif // VISTK_PROCESSES_IMAGE_GRAYSCALE_PROCESS_H
