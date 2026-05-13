// =============================================================================
// curvesExample - tcApp.cpp
// =============================================================================

#include "tcApp.h"
#include <string>

void tcApp::setup() {
    applyMode();
}

void tcApp::update() {}

// Apply the current mode/value to the global CurveStyle. Called whenever
// the user changes a slider via the keyboard.
void tcApp::applyMode() {
    if (tolMode_) setCurveTolerance(tolerance_);
    else          setCurveResolution(resolution_);
}

void tcApp::draw() {
    clear(0.10f, 0.11f, 0.13f);
    setColor(1.0f);

    // ------------------------------------------------------------------
    // Row 1 — Arc demo (sector fill + stroke variants)
    // ------------------------------------------------------------------
    float ax = 80, ay = 100;
    drawBitmapString("drawArc — fill (sector) / stroke (curve)", ax, ay - 30);

    // Filled pie sector spanning ~270 deg
    setColor(0.9f, 0.4f, 0.4f);
    drawArc(ax + 60, ay + 60, 60, 0.0f, HALF_TAU + QUARTER_TAU);

    // Stroked arc — 180 deg upper hemisphere
    noFill();
    setColor(0.4f, 0.9f, 0.6f);
    drawArc(ax + 220, ay + 60, 60, HALF_TAU, TAU);
    fill();

    // Filled donut chunk: outer arc + inner arc + line caps
    setColor(0.7f, 0.5f, 0.9f);
    beginShape();
    appendArc(ax + 380, ay + 60, 60, -QUARTER_TAU * 0.5f, QUARTER_TAU * 0.5f);
    appendArc(ax + 380, ay + 60, 35,  QUARTER_TAU * 0.5f, -QUARTER_TAU * 0.5f);
    endShape(true);

    // ------------------------------------------------------------------
    // Row 2 — Bezier demos (cubic + quadratic + N-th order)
    // ------------------------------------------------------------------
    float by = 280;
    setColor(1.0f);
    drawBitmapString("drawBezier — cubic / quadratic / N-th order", 80, by - 20);

    // Cubic
    setColor(0.95f, 0.7f, 0.3f);
    drawBezier(Vec3{80,  by + 80, 0},
               Vec3{180, by - 30, 0},
               Vec3{280, by + 180, 0},
               Vec3{380, by + 50, 0});
    // Visualise control polygon (faint)
    setColor(1.0f, 1.0f, 1.0f, 0.25f);
    drawLine(80, by + 80,   180, by - 30);
    drawLine(180, by - 30,  280, by + 180);
    drawLine(280, by + 180, 380, by + 50);

    // Quadratic
    setColor(0.4f, 0.85f, 0.95f);
    drawBezier(Vec3{440, by + 80, 0},
               Vec3{530, by - 40, 0},
               Vec3{620, by + 80, 0});

    // N-th order (5 control points)
    setColor(0.95f, 0.45f, 0.7f);
    std::vector<Vec3> pts = {
        Vec3{680,  by + 80,  0},
        Vec3{740,  by - 20,  0},
        Vec3{800,  by + 160, 0},
        Vec3{860,  by - 30,  0},
        Vec3{920,  by + 80,  0},
        Vec3{980,  by + 30,  0},
    };
    drawBezier(pts);

    // ------------------------------------------------------------------
    // Row 3 — Catmull-Rom (drawCurve) — smooth interpolation through pts
    // ------------------------------------------------------------------
    float cy = 480;
    setColor(1.0f);
    drawBitmapString("drawCurve — Catmull-Rom (passes through inner points)", 80, cy - 20);

    Vec3 c0{80,  cy + 60,  0};
    Vec3 c1{200, cy + 90,  0};
    Vec3 c2{320, cy + 30,  0};
    Vec3 c3{460, cy + 100, 0};
    setColor(0.85f, 0.95f, 0.4f);
    drawCurve(c0, c1, c2, c3);

    // Mark the four input points so it's obvious the spline passes through
    // c1 and c2 (and uses c0/c3 only as tangent influences).
    setColor(1.0f, 1.0f, 1.0f, 0.7f);
    for (auto& p : {c0, c1, c2, c3}) drawCircle(p.x, p.y, 4);

    // ------------------------------------------------------------------
    // Side panel — same circle at different curve-quality settings
    // ------------------------------------------------------------------
    pushStyle();
    setColor(1.0f);
    drawBitmapString("tolerance ladder", 600, cy - 20);
    float y = cy + 30;
    for (float tol : {2.0f, 0.5f, 0.1f}) {
        setCurveTolerance(tol);
        noFill(); setColor(0.4f, 0.85f, 0.95f);
        drawCircle(640, y, 30);
        fill(); setColor(1.0f);
        drawBitmapString("tol=" + std::to_string(tol), 685, y);
        y += 70;
    }

    drawBitmapString("resolution ladder", 850, cy - 20);
    y = cy + 30;
    for (int n : {6, 12, 32}) {
        setCurveResolution(n);
        noFill(); setColor(0.95f, 0.7f, 0.3f);
        drawCircle(890, y, 30);
        fill(); setColor(1.0f);
        drawBitmapString("res=" + std::to_string(n), 935, y);
        y += 70;
    }
    popStyle();

    drawHud();
}

void tcApp::drawHud() {
    setColor(1.0f, 1.0f, 1.0f, 0.85f);
    std::string line1 = std::string("mode: ") + (tolMode_ ? "TOLERANCE" : "RESOLUTION");
    std::string line2 = tolMode_
        ? ("global tolerance = " + std::to_string(tolerance_) + " px  [T / Shift+T]")
        : ("global resolution = " + std::to_string(resolution_) + "  [R / Shift+R]");
    std::string line3 = "M = toggle mode";
    drawBitmapString(line1, 20, 30);
    drawBitmapString(line2, 20, 48);
    drawBitmapString(line3, 20, 66);
}

void tcApp::keyPressed(int key) {
    switch (key) {
        case 'T': tolerance_ = std::max(0.01f,  tolerance_ * 0.5f);  applyMode(); break;
        case 't': tolerance_ = std::min(20.0f,  tolerance_ * 2.0f);  applyMode(); break;
        case 'R': resolution_ = std::max(3,    resolution_ - 2);    applyMode(); break;
        case 'r': resolution_ = std::min(256,  resolution_ + 2);    applyMode(); break;
        case 'm': case 'M': tolMode_ = !tolMode_; applyMode(); break;
    }
}
