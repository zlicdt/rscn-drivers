# RSCN Drivers - TODO List

## Phase 1: Project Setup

- [x] Initialize CMake project structure (CMakeLists.txt)
- [x] Set up Qt6 Widgets dependencies
- [x] Create basic directory layout (`src/`, `resources/`, `assets/`)
- [x] Create `.desktop` file for application launcher
- [x] Create polkit policy file (for root privilege escalation)
- [x] Add application icon and resources (.qrc)

## Phase 2: Hardware Detection Backend

- [x] Detect GPU hardware via `lspci` parsing
  - [x] Identify vendor (Intel / AMD / NVIDIA)
  - [x] Identify specific model and architecture generation
  - [x] Determine PCI ID for precise driver matching
- [x] Detect currently loaded kernel driver (`lspci -k`)
- [x] Detect currently installed driver packages (`pacman -Q`)
- [x] Detect available driver packages from repos (`pacman -Ss`)
- [x] Build a driver recommendation engine based on:
  - [x] GPU vendor + architecture → recommended packages
  - [x] Intel: mesa, vulkan-intel, intel-media-driver, etc.
  - [x] AMD: mesa, xf86-video-amdgpu, vulkan-radeon, etc.
  - [x] NVIDIA: nvidia-dkms / nvidia / nvidia-lts + utils
  - [x] NVIDIA legacy: nvidia-470xx-dkms (AUR)
  - [x] Nouveau fallback: xf86-video-nouveau

## Phase 3: Driver Database / Profile System

- [x] Define a driver profile data structure:
  - Driver name (display)
  - Package list (required + optional)
  - Source (pacman / AUR)
  - Type (proprietary / open-source)
  - Recommended flag
  - Supported GPU architectures / PCI IDs
- [x] Create driver profiles for:
  - [x] Intel modern (Broadwell+)
  - [x] Intel legacy (pre-Broadwell)
  - [x] AMD open-source (GCN+)
  - [x] AMD legacy (pre-GCN, xf86-video-ati)
  - [x] AMD PRO (amdgpu-pro, AUR)
  - [x] NVIDIA proprietary (Maxwell+, nvidia-dkms)
  - [x] NVIDIA LTS kernel variant
  - [x] NVIDIA legacy Kepler (nvidia-470xx-dkms, AUR)
  - [x] NVIDIA open-source (Nouveau)
- [x] Determine installed status for each profile

## Phase 4: Package Management Backend

- [ ] Implement pacman wrapper for:
  - [ ] Install packages (`pacman -S --noconfirm`)
  - [ ] Remove packages (`pacman -Rns`)
  - [ ] Check if package is installed (`pacman -Q`)
  - [ ] Check if package is available (`pacman -Si`)
- [ ] Implement AUR helper integration (yay/paru):
  - [ ] Detect available AUR helper
  - [ ] Install AUR packages
  - [ ] Remove AUR packages
- [ ] Implement privilege escalation via pkexec/polkit
- [ ] Handle post-install steps:
  - [ ] NVIDIA: remove `kms` from mkinitcpio HOOKS
  - [ ] Regenerate initramfs (`mkinitcpio -P`)
  - [ ] Regenerate GRUB config (`grub-mkconfig`)
  - [ ] Prompt for reboot

## Phase 5: Main Window UI

- [ ] Create main window layout (similar to Ubuntu's Additional Drivers)
  - [ ] Header: application title + device info summary
  - [ ] Device list panel (left or top section)
    - [ ] Show detected GPU device(s) with icon + model name
  - [ ] Driver options panel (right or center section)
    - [ ] Radio button group for each available driver option
    - [ ] Labels: driver name, version, type (proprietary/open-source)
    - [ ] "Recommended" badge for best-match driver
    - [ ] "Currently in use" indicator
    - [ ] Package details (expandable or tooltip)
  - [ ] Action buttons:
    - [ ] "Apply Changes" button (install/switch driver)
    - [ ] "Revert" button (undo selection)
  - [ ] Status bar showing current operation
- [ ] Loading/splash screen while scanning hardware
- [ ] "No proprietary drivers in use" / "No devices found" empty state

## Phase 6: Progress & Feedback UI

- [ ] Progress dialog for package operations
  - [ ] Show real-time pacman output in a log viewer
  - [ ] Progress bar (indeterminate or parsed)
  - [ ] Cancel button (if possible)
- [ ] Success / failure result dialog
- [ ] Reboot prompt dialog after driver changes
- [ ] Error handling dialogs:
  - [ ] pacman lock file busy
  - [ ] Network unavailable
  - [ ] Package not found
  - [ ] Dependency conflicts

## Phase 7: System Integration

- [ ] Polkit policy for privilege escalation
- [ ] .desktop file with appropriate categories
- [ ] Application icon (SVG + multiple PNG sizes)
- [ ] PKGBUILD for Arch packaging
- [ ] Integration with system tray notifications (optional)
- [ ] Translations / i18n support via Qt Linguist (optional)

## Phase 8: Testing & QA

- [ ] Test on Intel-only system
- [ ] Test on AMD GPU system
- [ ] Test on NVIDIA GPU system
- [ ] Test on hybrid GPU system (e.g., Intel + NVIDIA)
- [ ] Test driver switching (e.g., nouveau → nvidia)
- [ ] Test error handling (no network, locked pacman, etc.)
- [ ] Verify post-install steps (mkinitcpio, grub)

## Phase 9: Polish & Release

- [ ] Code cleanup and documentation
- [ ] README update with build instructions and screenshots
- [ ] Add license headers to source files
- [ ] Create release PKGBUILD
- [ ] Tag v1.0.0 release
