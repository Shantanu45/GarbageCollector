#pragma once

#include <stdint.h>

/**
 * Object header.
 *
 * The header stores meta-information for the Collector, and Allocator
 * purposes. It's located in the word prior to the payload pointer.
 *
 */
struct ObjectHeader {
    /**
     * The forwarding address (using by moving/copying collectors).
     */
    uint16_t forward = 0x0;

    /**
     * The block size.
     */
    uint8_t size;

    /**
     * Specific GC data.
     */
    union {
        /**
         * Used by Mark-Sweep GC during trace phase.
         */
        uint8_t mark;

        /**
         * Reference counter in RC collector (up to 255 references).
         */
        uint8_t rc;
    };

    operator uint32_t() { return toInt(); }
    uint32_t toInt() { return *reinterpret_cast<uint32_t*>(this); }
};