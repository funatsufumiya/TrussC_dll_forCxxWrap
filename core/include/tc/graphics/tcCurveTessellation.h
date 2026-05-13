#pragma once

// =============================================================================
// tcCurveTessellation.h
//
// Adaptive tessellation for circles, arcs, and Bezier curves.
//
// Two modes:
//   - Tolerance: pick the segment count so the chord-to-curve sagitta is
//                <= tolerance pixels in screen space. Default (0.1 px).
//   - Resolution: fixed segment count, ignoring radius. Opt-in for users
//                who want to bound vertex cost or get a chunky look.
//
// Most users only touch the public API (setCurveTolerance / setCurveResolution
// in tcRenderContext.h). The functions here are the math primitives those
// public functions and individual draw* / Path methods are built on.
// =============================================================================

#include "tcMath.h"   // TAU, HALF_TAU
#include <algorithm>
#include <cmath>

namespace trussc {

struct CurveStyle {
    enum class Mode { Tolerance, Resolution };
    Mode  mode       = Mode::Tolerance;
    float tolerance  = 0.1f;   // pixels in screen space
    int   resolution = 60;     // fallback / fixed-mode value
};

// Lower / upper guards for segment counts. The lower keeps polygons looking
// like polygons even at tiny radii; the upper prevents pathological vertex
// floods from bad input (radius=1e9, tolerance=1e-6, ...).
inline constexpr int kMinCircleSegments = 6;
inline constexpr int kMaxCircleSegments = 1024;

// Number of edges to approximate a full circle of `radius` so that the
// chord-to-arc sagitta stays under `tolerance`. Derivation:
//
//   Sagitta s of an arc spanning angle theta is r*(1 - cos(theta/2)).
//   For N segments, theta = TAU/N. Setting s <= tolerance:
//
//     1 - cos(theta/2) <= tolerance/r
//     theta/2          >= acos(1 - tolerance/r)
//     N                >= HALF_TAU / acos(1 - tolerance/r)
//
// Both inputs are pre-divided by the current scale by the caller.
inline int segmentsForCircle(float radius, float tolerance) {
    if (radius <= 0.0f) return 0;
    if (tolerance <= 0.0f) return kMaxCircleSegments;
    if (tolerance >= radius) return kMinCircleSegments;
    float n = std::ceil(HALF_TAU / std::acos(1.0f - tolerance / radius));
    int   ni = (int)n;
    if (ni < kMinCircleSegments) ni = kMinCircleSegments;
    if (ni > kMaxCircleSegments) ni = kMaxCircleSegments;
    return ni;
}

// Edges to span an arc of `angleSpan` radians at the given tolerance.
// At least 2 segments so a non-zero arc always has interior points.
inline int segmentsForArc(float radius, float angleSpan, float tolerance) {
    if (radius <= 0.0f || angleSpan == 0.0f) return 0;
    int full = segmentsForCircle(radius, tolerance);
    int n = (int)std::ceil(full * std::abs(angleSpan) / TAU);
    return std::max(2, n);
}

} // namespace trussc
