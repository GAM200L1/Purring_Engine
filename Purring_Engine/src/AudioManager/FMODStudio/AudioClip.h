/*!***********************************************************************************
 \project  Purring Engine
 \module   CSD2401-A
 \file     AudioClip.h
 \date     2-03-2024

 \author               Hans (You Yang) ONG
 \par      email:      youyang.o@digipen.edu

 \brief    FMOD Studio Audio Clip setup.

 All content (c) 2023 DigiPen Institute of Technology Singapore. All rights reserved.
*************************************************************************************/
#pragma once

#include <string>

namespace PE
{
    namespace Audio
    {

        struct AudioClip
        {
            std::string clipPath;  // Path to the audio file

            /*!***********************************************************************************
             \brief Constructor to initialize an AudioClip with a specified file path.

             \param[in] r_path - The path to the audio file.
            *************************************************************************************/
            AudioClip(const std::string& r_path) : clipPath(r_path) {}

            /*!***********************************************************************************
             \brief Default constructor for creating an empty AudioClip.
            *************************************************************************************/
            AudioClip() = default;

            /*!***********************************************************************************
             \brief Copy constructor for creating a copy of an existing AudioClip.

             \param[in] other - The AudioClip to copy.
            *************************************************************************************/
            AudioClip(const AudioClip& r_other) = default;

            /*!***********************************************************************************
             \brief Move constructor for transferring ownership of an AudioClip.

             \param[in] other - The AudioClip to move.
            *************************************************************************************/
            AudioClip(AudioClip&& r_other) noexcept = default;

            /*!***********************************************************************************
             \brief Copy assignment operator for assigning the value of one AudioClip to another.

             \param[in] other - The AudioClip to assign from.
             \return A reference to the modified AudioClip.
            *************************************************************************************/
            AudioClip& operator=(const AudioClip&) = default;

            /*!***********************************************************************************
             \brief Move assignment operator for transferring ownership of an AudioClip.

             \param[in] other - The AudioClip to move assign from.
             \return A reference to the modified AudioClip.
            *************************************************************************************/
            AudioClip& operator=(AudioClip&&) noexcept = default;

            /*!***********************************************************************************
             \brief Default destructor for AudioClip.
            *************************************************************************************/
            ~AudioClip() = default;
        };

    } // namespace Audio

} // namespace PE
