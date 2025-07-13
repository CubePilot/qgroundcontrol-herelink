# UTG (Ultrasonic Thickness Gauge) UI Mockups

This document provides detailed ASCII mockups of all the UTG UI components implemented in QGroundControl.

## Table of Contents
1. [Enhanced Flight View Widget](#enhanced-flight-view-widget)
2. [UTG Settings Panel](#utg-settings-panel)
3. [Settings Dialog in Flight View](#settings-dialog-in-flight-view)
4. [Status Indicators](#status-indicators)
5. [Responsive Layouts](#responsive-layouts)

---

## Enhanced Flight View Widget

### Main Widget Layout (Connected State)
```
┌─────────────────────────────────────────────────────────────┐
│ UTG Thickness                                            ● │ ← Green status dot
├─────────────────────────────────────────────────────────────┤
│                                                             │
│                        25.43 mm                            │ ← Large reading display
│                                                             │
├─────────────────────────────────────────────────────────────┤
│ Status: Connected                                           │ ← Status text
├─────────────────────────────────────────────────────────────┤
│ [  Measure  ] [Continuous] [ Settings ]                    │ ← Control buttons
├─────────────────────────────────────────────────────────────┤
│ Gain: [50] Velocity: [5920]                                │ ← Quick settings
└─────────────────────────────────────────────────────────────┘
```

### Widget States

#### Disconnected State
```
┌─────────────────────────────────────────────────────────────┐
│ UTG Thickness                                            ● │ ← Orange status dot
├─────────────────────────────────────────────────────────────┤
│                                                             │
│                    Disconnected                            │ ← Status message
│                                                             │
├─────────────────────────────────────────────────────────────┤
│ Status: Connecting...                                       │
├─────────────────────────────────────────────────────────────┤
│ [  Measure  ] [Continuous] [ Settings ]                    │ ← Buttons disabled
├─────────────────────────────────────────────────────────────┤
│ Quick settings hidden when disconnected                    │
└─────────────────────────────────────────────────────────────┘
```

#### Disabled State
```
┌─────────────────────────────────────────────────────────────┐
│ UTG Thickness                                            ● │ ← Grey status dot
├─────────────────────────────────────────────────────────────┤
│                                                             │
│                      Disabled                              │ ← Status message
│                                                             │
├─────────────────────────────────────────────────────────────┤
│ Status: UTG not enabled                                     │
├─────────────────────────────────────────────────────────────┤
│ [  Measure  ] [Continuous] [ Settings ]                    │ ← Only Settings enabled
├─────────────────────────────────────────────────────────────┤
│ Enable UTG in settings to use                              │
└─────────────────────────────────────────────────────────────┘
```

#### Measuring State (Continuous Mode)
```
┌─────────────────────────────────────────────────────────────┐
│ UTG Thickness                                            ● │ ← Blinking green dot
├─────────────────────────────────────────────────────────────┤
│                                                             │
│                      12.87 mm                              │ ← Live updating
│                                                             │
├─────────────────────────────────────────────────────────────┤
│ Status: Measuring (Continuous)                             │
├─────────────────────────────────────────────────────────────┤
│ [   Stop    ] [Continuous] [ Settings ]                    │ ← Stop button active
├─────────────────────────────────────────────────────────────┤
│ Gain: [50] Velocity: [5920]                                │ ← Live adjustable
└─────────────────────────────────────────────────────────────┘
```

---

## UTG Settings Panel

### Complete Settings Panel Layout
```
═══════════════════════════════════════════════════════════════════════════════
                    UTG (Ultrasonic Thickness Gauge) Settings
═══════════════════════════════════════════════════════════════════════════════

┌─ General Settings ──────────────────────────────────────────────────────────┐
│                                                                             │
│  ☑ Enable UTG                                                              │
│  ☑ Auto Connect                                                            │
│                                                                             │
└─────────────────────────────────────────────────────────────────────────────┘

┌─ Serial Port Configuration ─────────────────────────────────────────────────┐
│                                                                             │
│  Serial Port:     [COM3                           ▼]                       │
│  Baud Rate:       [9600                           ▼]                       │
│  Data Bits:       [8                              ▼]                       │
│  Stop Bits:       [1                              ▼]                       │
│  Parity:          [None                           ▼]                       │
│  Flow Control:    ☐ Enable Hardware Flow Control                           │
│                                                                             │
└─────────────────────────────────────────────────────────────────────────────┘

┌─ Measurement Settings ──────────────────────────────────────────────────────┐
│                                                                             │
│  Unit:            [mm                             ▼]                       │
│  Mode:            [Pulse-Echo                     ▼]                       │
│  Sound Velocity:  [5920.0                           ] m/s                  │
│  Zero Offset:     [0.000                            ]                      │
│  Gain (%):        [50                               ]                      │
│  Threshold (%):   [50                               ]                      │
│  Pulse Width:     [100                              ] ns                   │
│  Frequency:       [5.0                              ] MHz                  │
│  Precision:       [2                                ] decimal places       │
│                                                                             │
└─────────────────────────────────────────────────────────────────────────────┘

┌─ Material & Calibration ────────────────────────────────────────────────────┐
│                                                                             │
│  Material Type:   [Steel                          ▼]                       │
│  Calib. Mode:     [One Point                      ▼]                       │
│  Calib. Value:    [10.000                           ]                      │
│  Temp. Comp.:     ☑ Enable Temperature Compensation                        │
│                                                                             │
└─────────────────────────────────────────────────────────────────────────────┘

┌─ Alerts & Logging ──────────────────────────────────────────────────────────┐
│                                                                             │
│  Enable Alerts:   ☑                                                        │
│  Alert Sound:     ☑                                                        │
│  Min Alert:       [0.000                            ]                      │
│  Max Alert:       [100.000                          ]                      │
│  Log Measure.:    ☑                                                        │
│  Auto Save:       ☐                                                        │
│                                                                             │
└─────────────────────────────────────────────────────────────────────────────┘
```

### Dropdown Menu Examples

#### Serial Port Dropdown
```
Serial Port:  [COM3                           ▼]
              ┌─────────────────────────────────┐
              │ COM1                            │
              │ COM2                            │
              │ COM3                         ✓  │ ← Selected
              │ COM4                            │
              │ /dev/ttyUSB0                    │
              │ /dev/ttyUSB1                    │
              └─────────────────────────────────┘
```

#### Measurement Unit Dropdown
```
Unit:         [mm                             ▼]
              ┌─────────────────────────────────┐
              │ mm                           ✓  │ ← Selected
              │ inch                            │
              │ mil                             │
              └─────────────────────────────────┘
```

#### Material Type Dropdown
```
Material:     [Steel                          ▼]
              ┌─────────────────────────────────┐
              │ Steel                        ✓  │ ← Selected (5920 m/s)
              │ Aluminum                        │ ← (6420 m/s)
              │ Copper                          │ ← (4760 m/s)
              │ Plastic                         │ ← (2700 m/s)
              │ Custom                          │ ← User defined
              └─────────────────────────────────┘
```

---

## Settings Dialog in Flight View

### Main Dialog Layout
```
┌─ UTG Settings ──────────────────────────────────────────────────────────────┐
│                                                                      [×]    │
│                                                                             │
│ ┌─ Connection ─────────────────────────────────────────────────────────────┐ │
│ │                                                                         │ │
│ │ Status:       Connected                                              ●  │ │ ← Green dot
│ │ Serial Port:  [COM3                           ▼]                        │ │
│ │                                                                         │ │
│ │                            [Disconnect]                                 │ │
│ │                                                                         │ │
│ └─────────────────────────────────────────────────────────────────────────┘ │
│                                                                             │
│ ┌─ Measurement ───────────────────────────────────────────────────────────┐ │
│ │                                                                         │ │
│ │ Unit:         [mm                             ▼]                        │ │
│ │ Mode:         [Pulse-Echo                     ▼]                        │ │
│ │ Velocity:     [5920.0                           ] m/s                   │ │
│ │ Offset:       [0.000                            ]                       │ │
│ │                                                                         │ │
│ └─────────────────────────────────────────────────────────────────────────┘ │
│                                                                             │
│ ┌─ Signal Parameters ─────────────────────────────────────────────────────┐ │
│ │                                                                         │ │
│ │ Gain (%):     [50                               ]                       │ │
│ │ Threshold:    [50                               ]                       │ │
│ │ Frequency:    [5.0                              ] MHz                   │ │
│ │ Pulse Width:  [100                              ] ns                    │ │
│ │                                                                         │ │
│ └─────────────────────────────────────────────────────────────────────────┘ │
│                                                                             │
│ ┌─ Calibration ───────────────────────────────────────────────────────────┐ │
│ │                                                                         │ │
│ │ Material:     [Steel                          ▼]                        │ │
│ │ Mode:         [One Point                      ▼]                        │ │
│ │ Ref Value:    [10.000                           ]                       │ │
│ │                                                                         │ │
│ │ [Zero Cal] [Velocity Cal] [Reset UTG]                                   │ │
│ │                                                                         │ │
│ └─────────────────────────────────────────────────────────────────────────┘ │
│                                                                             │
│                                    [OK] [Cancel]                           │
└─────────────────────────────────────────────────────────────────────────────┘
```

### Calibration Dialog States

#### Zero Calibration in Progress
```
┌─ Zero Calibration ──────────────────────────────────────────────────────────┐
│                                                                             │
│  Please ensure the probe is in air (no contact with material)              │
│                                                                             │
│  ┌─────────────────────────────────────────────────────────────────────┐   │
│  │ ████████████████████████████████████████████████████████████████████ │   │ ← Progress bar
│  │                        Calibrating... 75%                          │   │
│  └─────────────────────────────────────────────────────────────────────┘   │
│                                                                             │
│                              [Cancel]                                      │
└─────────────────────────────────────────────────────────────────────────────┘
```

#### Velocity Calibration Dialog
```
┌─ Velocity Calibration ──────────────────────────────────────────────────────┐
│                                                                             │
│  Place probe on material with known thickness                              │
│                                                                             │
│  Known Thickness:  [25.40                           ] mm                   │
│  Current Reading:  [24.87                           ] mm                   │
│  Velocity Adjust:  [5856                            ] m/s                  │
│                                                                             │
│  ┌─ Instructions ─────────────────────────────────────────────────────────┐ │
│  │ 1. Enter the known thickness of your test piece                       │ │
│  │ 2. Place the probe firmly on the material                             │ │
│  │ 3. Click "Calibrate" to adjust the velocity                           │ │
│  │ 4. Verify the reading matches the known thickness                     │ │
│  └─────────────────────────────────────────────────────────────────────────┘ │
│                                                                             │
│                        [Calibrate] [Cancel]                                │
└─────────────────────────────────────────────────────────────────────────────┘
```

---

## Status Indicators

### Connection Status Indicators
```
● Green    = Connected and operational
● Orange   = Enabled but not connected
● Grey     = Disabled
● Red      = Error state
● Blinking = Active measurement in progress
```

### Status Text Examples
```
┌─ Status Messages ───────────────────────────────────────────────────────────┐
│                                                                             │
│ ✓ Connected                          (Green text)                          │
│ ⚠ Connecting...                      (Orange text)                         │
│ ⚠ Disconnected                       (Orange text)                         │
│ ✗ Error: Port not found              (Red text)                            │
│ ✗ Error: Communication timeout       (Red text)                            │
│ ⟳ Measuring (Continuous)             (Blue text, animated)                 │
│ ⟳ Calibrating...                     (Blue text, animated)                 │
│ ⚪ Disabled                           (Grey text)                           │
│                                                                             │
└─────────────────────────────────────────────────────────────────────────────┘
```

---

## Responsive Layouts

### Mobile/Tablet Layout (Compact)
```
┌─────────────────────────────────┐
│ UTG                          ● │
├─────────────────────────────────┤
│                                 │
│           25.43 mm              │
│                                 │
├─────────────────────────────────┤
│ Connected                       │
├─────────────────────────────────┤
│ [Measure] [⚙]                  │ ← Compact buttons
├─────────────────────────────────┤
│ G:[50] V:[5920]                 │ ← Abbreviated labels
└─────────────────────────────────┘
```

### Desktop Layout (Full)
```
┌─────────────────────────────────────────────────────────────┐
│ UTG Thickness Gauge                                      ● │
├─────────────────────────────────────────────────────────────┤
│                                                             │
│                        25.43 mm                            │
│                                                             │
├─────────────────────────────────────────────────────────────┤
│ Status: Connected to COM3 at 9600 baud                     │
├─────────────────────────────────────────────────────────────┤
│ [Single Measure] [Continuous] [Advanced Settings]          │
├─────────────────────────────────────────────────────────────┤
│ Gain: [50] %  Sound Velocity: [5920] m/s  Temp: 23.5°C     │
└─────────────────────────────────────────────────────────────┘
```

### Settings Panel Responsive Sections

#### Collapsed Sections (Mobile)
```
┌─ UTG Settings ──────────────────┐
│                                 │
│ ▶ General                       │ ← Collapsed
│ ▼ Serial Port                   │ ← Expanded
│   Port: [COM3        ▼]        │
│   Baud: [9600        ▼]        │
│ ▶ Measurement                   │ ← Collapsed
│ ▶ Calibration                   │ ← Collapsed
│ ▶ Alerts                        │ ← Collapsed
│                                 │
└─────────────────────────────────┘
```

#### Expanded Sections (Desktop)
```
All sections visible simultaneously with full labels and spacing
```

---

## Interactive Elements

### Button States
```
┌─ Button States ─────────────────────────────────────────────────────────────┐
│                                                                             │
│ [  Normal  ]  ← Default state                                               │
│ [  Hover   ]  ← Mouse over (highlighted)                                    │
│ [  Active  ]  ← Currently pressed                                           │
│ [  Disabled]  ← Greyed out, not clickable                                   │
│ [●Measuring]  ← Active state with indicator                                 │
│                                                                             │
└─────────────────────────────────────────────────────────────────────────────┘
```

### Input Field States
```
┌─ Input Field States ────────────────────────────────────────────────────────┐
│                                                                             │
│ [Normal input field        ]  ← Default                                     │
│ [Focused input field       ]  ← Blue border when selected                  │
│ [Error: Invalid value      ]  ← Red border for errors                      │
│ [Disabled field            ]  ← Greyed out                                  │
│                                                                             │
└─────────────────────────────────────────────────────────────────────────────┘
```

### Measurement Display Variations
```
┌─ Measurement Display States ────────────────────────────────────────────────┐
│                                                                             │
│                        25.43 mm                                            │ ← Normal
│                       ⟳ 25.43 mm                                           │ ← Measuring
│                       ⚠ No Signal                                          │ ← Warning
│                       ✗ Error                                              │ ← Error
│                       -- . -- mm                                           │ ← No data
│                                                                             │
└─────────────────────────────────────────────────────────────────────────────┘
```

---

## Color Scheme Reference

```
┌─ QGroundControl Color Palette ──────────────────────────────────────────────┐
│                                                                             │
│ ● Green     = #4CAF50  (Connected, Success)                                │
│ ● Orange    = #FF9800  (Warning, Connecting)                               │
│ ● Red       = #F44336  (Error, Critical)                                   │
│ ● Blue      = #2196F3  (Active, Measuring)                                 │
│ ● Grey      = #9E9E9E  (Disabled, Inactive)                                │
│ ● Dark Grey = #424242  (Text, Borders)                                     │
│ ● Light     = #FAFAFA  (Background)                                        │
│                                                                             │
└─────────────────────────────────────────────────────────────────────────────┘
```

This README provides comprehensive visual mockups of all UTG UI components. The actual implementation will render with proper QGroundControl styling, fonts, and colors.
