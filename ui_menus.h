#pragma once

// ── Onboarding state ──────────────────────────────────────────────────────────
extern bool showOnboarding;
extern int  onboardingStep;

// ── Color menu state ──────────────────────────────────────────────────────────
extern bool showColorMenu;
extern int  selectedTheme;

// ── Functions ─────────────────────────────────────────────────────────────────
void DrawOnboarding();
void DrawColorMenu();
void DrawWidgetMenu();
