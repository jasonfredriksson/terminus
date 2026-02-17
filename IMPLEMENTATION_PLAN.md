# Retro CRT Dashboard - Implementation Plan

## Current Status
- ✅ Basic dashboard with customizable widgets
- ✅ CRT shader effects (scanlines, curvature, glow)
- ✅ Real system monitoring (CPU, RAM, Disk via Windows APIs)
- ✅ Widget customization menu
- ⚠️ Color theme system (needs fixing)
- ⚠️ Onboarding flow (being skipped)

## Issues to Fix

### Priority 1: Critical Bugs
1. **Color Theme Selection Menu**
   - **Issue**: Accessing menu automatically applies Green Phosphor
   - **Root Cause**: Need to investigate why theme applies on menu open instead of on ENTER
   - **Fix**: Separate menu display from theme application logic
   - **Test**: Navigate to color menu, browse themes without applying, press ENTER to apply

2. **Onboarding Flow**
   - **Issue**: Onboarding is being skipped even when `isFirstRun = true`
   - **Root Cause**: Check initialization order and flag logic
   - **Fix**: Ensure onboarding shows on first run, save preference to config file
   - **Test**: Delete config, restart app, verify onboarding appears

## Features to Implement

### Phase 1: Fix Existing Features (Priority: HIGH)

#### 1.1 Fix Color Theme System
- [ ] Debug why theme applies on menu open
- [ ] Ensure theme only applies on ENTER key
- [ ] Add visual preview without applying
- [ ] Test all 5 color themes
- [ ] Verify colors change across all UI elements

#### 1.2 Fix Onboarding
- [ ] Set `isFirstRun = true` by default
- [ ] Verify onboarding displays on startup
- [ ] Test welcome screen navigation
- [ ] Test color selection in onboarding
- [ ] Save "completed onboarding" flag to config

### Phase 2: Integrate Existing System Monitoring (Priority: HIGH)

#### 2.1 Display Process Count
- [ ] Add process count widget to dashboard
- [ ] Format: "PROCESSES: 142 running"
- [ ] Update in real-time
- [ ] Make toggleable in widget menu

#### 2.2 Display System Uptime
- [ ] Add uptime widget to dashboard
- [ ] Format: "UPTIME: 2d 14h 32m"
- [ ] Convert seconds to readable format
- [ ] Make toggleable in widget menu

#### 2.3 Display Computer Name
- [ ] Add computer name to header/title area
- [ ] Format: "MAINFRAME: DESKTOP-ABC123"
- [ ] Show alongside [LIVE]/[SIM] indicator
- [ ] Make toggleable in widget menu

### Phase 3: Real Network Monitoring (Priority: MEDIUM)

#### 3.1 Implement Real Network Stats
- [ ] Add `GetNetworkStats()` function using Windows `GetIfTable2()`
- [ ] Calculate real bytes sent/received
- [ ] Compute real-time upload/download speeds (MB/s)
- [ ] Update network widget with real data when in LIVE mode

#### 3.2 Network Widget Enhancement
- [ ] Show current speed (instantaneous)
- [ ] Show average speed (over last 10 seconds)
- [ ] Show total data transferred (session)
- [ ] Add network interface name

### Phase 4: System Information View (Priority: MEDIUM)

#### 4.1 Create System Info Page
- [ ] Implement dedicated view when "SYSTEM INFORMATION" is selected
- [ ] Display:
  - OS Version (Windows 10/11 Build XXXXX)
  - CPU: Name, Cores, Threads
  - Total RAM (GB)
  - Total Disk Space (GB)
  - Computer Name
  - System Uptime
  - Total Processes
  - Network Interfaces
- [ ] Add "Press ESC to return" instruction
- [ ] Make it look retro with ASCII borders

### Phase 5: Configuration Persistence (Priority: MEDIUM)

#### 5.1 Config File System
- [ ] Create `config.ini` file format
- [ ] Save settings:
  - `theme=1` (selected color theme)
  - `first_run=false` (onboarding completed)
  - `widgets_cpu=true` (each widget state)
  - `widgets_ram=true`
  - `widgets_disk=true`
  - `widgets_network=true`
  - `widgets_anomaly=true`
  - `widgets_log=true`
  - `widgets_time=true`
- [ ] Load settings on startup
- [ ] Create default config if not exists

#### 5.2 Config Functions
- [ ] `SaveConfig()` - Write current state to file
- [ ] `LoadConfig()` - Read config and apply settings
- [ ] `CreateDefaultConfig()` - Generate default config.ini
- [ ] Call `SaveConfig()` when:
  - Theme changes
  - Widget toggled
  - Onboarding completed

### Phase 6: Additional Enhancements (Priority: LOW)

#### 6.1 More Widgets
- [ ] CPU Temperature (if available)
- [ ] GPU Usage (if available)
- [ ] Battery Status (for laptops)
- [ ] Date/Time widget (separate from system time)

#### 6.2 Network Diagnostics View
- [ ] Ping test to common servers (8.8.8.8, 1.1.1.1)
- [ ] Show latency
- [ ] Show packet loss
- [ ] Connection quality indicator

#### 6.3 Visual Enhancements
- [ ] Add more CRT shader options (intensity slider)
- [ ] Animated transitions between views
- [ ] Sound effects (optional, retro beeps)
- [ ] Custom font support (load user's TTF files)

## Testing Checklist

### Menu Navigation
- [ ] TAB opens/closes main menu
- [ ] UP/DOWN navigates menu options
- [ ] ENTER selects option
- [ ] All 6 menu options accessible
- [ ] ESC closes submenus without closing app

### Color Themes
- [ ] Can access color theme menu
- [ ] Can browse all 5 themes
- [ ] Preview shows correct colors
- [ ] ENTER applies selected theme
- [ ] ESC cancels without applying
- [ ] Theme persists after restart

### Widgets
- [ ] All widgets display correctly
- [ ] Can toggle each widget on/off
- [ ] Layout adjusts when widgets hidden
- [ ] Widget states persist after restart

### System Monitoring
- [ ] CPU usage updates in real-time
- [ ] RAM usage shows actual values
- [ ] Disk usage accurate
- [ ] Process count updates
- [ ] Uptime increments correctly
- [ ] Network stats show real data (when implemented)

### Onboarding
- [ ] Shows on first run
- [ ] Welcome screen displays
- [ ] Can navigate with ENTER/ESC
- [ ] Color selection works
- [ ] Doesn't show on subsequent runs
- [ ] Can be reset by deleting config

## Implementation Order

### Sprint 1: Fix Critical Bugs (1-2 hours)
1. Fix color theme menu application logic
2. Fix onboarding display
3. Test both features thoroughly

### Sprint 2: System Monitoring Integration (1 hour)
1. Add process count display
2. Add uptime display
3. Add computer name display
4. Test all new displays

### Sprint 3: Network Monitoring (1-2 hours)
1. Implement Windows network APIs
2. Calculate real-time speeds
3. Update network widget
4. Test accuracy

### Sprint 4: System Info View (1 hour)
1. Create dedicated system info page
2. Gather all system information
3. Format display nicely
4. Add navigation

### Sprint 5: Configuration System (1-2 hours)
1. Implement config file I/O
2. Save/load all settings
3. Test persistence
4. Handle missing/corrupt config files

### Sprint 6: Polish & Testing (1 hour)
1. Test all features end-to-end
2. Fix any remaining bugs
3. Update README with all features
4. Create user guide

## Notes
- Keep modular structure (separate .h/.cpp files)
- Use edit tools, not file recreation
- Test after each major change
- Document as we go
- Prioritize working features over perfect code

## Success Criteria
- ✅ All menu options work correctly
- ✅ Color themes apply properly
- ✅ All widgets functional and toggleable
- ✅ Real system monitoring accurate
- ✅ Configuration persists between sessions
- ✅ No crashes or major bugs
- ✅ Clean, maintainable code structure
