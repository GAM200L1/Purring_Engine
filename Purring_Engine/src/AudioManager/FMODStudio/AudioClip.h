#pragma once

#include <string>

namespace PE
{
    namespace Audio
    {

        struct AudioClip
        {
            std::string clipPath;  // Path to the audio file

            // Constructor to initialize AudioClip with a path
            AudioClip(const std::string& path) : clipPath(path) {}

            // Default constructor
            AudioClip() = default;

            // Copy constructor
            AudioClip(const AudioClip&) = default;

            // Move constructor
            AudioClip(AudioClip&&) noexcept = default;

            // Copy assignment operator
            AudioClip& operator=(const AudioClip&) = default;

            // Move assignment operator
            AudioClip& operator=(AudioClip&&) noexcept = default;

            // Destructor
            ~AudioClip() = default;
        };

    } // namespace Audio

} // namespace PE
