# BareMetal-KV11-Calculator
A four-function integer calculator written in bare-metal C for the NXP Kinetis KV11 (MKV11Z7). Input comes from a 4x4 matrix keypad and results appear on a 16x2 HD44780-style character LCD. The LCD driver (4-bit mode) and keypad scanner are written from scratch using register-level GPIO access.

**DEMO**

<img width="685" height="909" alt="Screenshot 2026-11-07 173008" src="https://github.com/user-attachments/assets/b97a7aed-f5f0-41b5-9c1a-852e1f2a318d" />

## Hardware
| Part | Notes |
| :--- | :--- |
| **MCU** | NXP Kinetis KV11 (FRDM-KV11Z) |
| **Display** | 16x2 HD44780-compatible character LCD (4-bit mode, 5V logic) |
| **Input** | 4x4 matrix keypad |
| **Misc** | 10k potentiometer (LCD contrast), 2x 10Ω resistors (Backlight current limiting) |

## Wiring

### LCD
| LCD pin | MCU / Power |
| :--- | :--- |
| D4 | PTD0 |
| D5 | PTD1 |
| D6 | PTD2 |
| D7 | PTD3 |
| E | PTD4 |
| RS | PTD5 |
| RW | PTA1 (Held Low for Write) |
| V0 (Contrast) | 10k Potentiometer Wiper |
| A (Backlight +) | 5V via 2x 10Ω resistors |

### Keypad
| Signal | MCU pin | Direction |
| :--- | :--- | :--- |
| Row 0-3 (driven) | PTC0-PTC3 | Output |
| Col 0-3 (read) | PTC4-PTC7 | Input (internal pull-up) |

## Operation

**Keypad Layout:**
* `1` `2` `3` `A` = Add
* `4` `5` `6` `B` = Subtract
* `7` `8` `9` `C` = Clear
* `*` `0` `#` `D` = Divide (`*` = Multiply, `#` = Equals)

**Rules & Input Guards:**
* Numbers are limited to 3 digits to fit the 8-column display limit.
* Operators and `=` are ignored if no number is actively entered.
* Dividing by zero triggers an `ERROR` state.
* Entering a new digit after an equation automatically clears the screen and starts a new calculation.

## Architecture

* **LCD Driver:** Custom 4-bit implementation. Splits bytes into nibbles, toggles RS for instruction/data, holds RW low (write-only), and pulses Enable to latch.
* **Keypad Scanner:** Pulls one row LOW at a time. Reads all four columns. A pressed key grounds the column, triggering the internal pull-up and indexing the char map.
* **State Machine:** 3-stage FSM.
  1. `STATE 1`: Build first operand.
  2. `STATE 2`: Operator selected, build second operand.
  3. `STATE 3`: Calculate, display result, lock until reset.

## Hardware Verification

| Test Case | Expected Output | Result |
| :--- | :--- | :--- |
| `12A34#` | `46` | Pass |
| `5B9#` | `-4` | Pass |
| `7*8#` | `56` | Pass |
| `9D3#` | `3` | Pass |
| `9D0#` | `ERROR` | Pass |
| `9999` | Caps at `999` | Pass |
| `#`, `A`, or `5A#` | Ignored | Pass |
| `5A3C` | Screen clears | Pass |

## Known Limitations & Planned Improvements

**Current limitations**
- Integer-only operation (no decimals or negative inputs).
- No chained operations (e.g., `1+2+3#`).
- Delays are software loops, so the keypad is not scanned during LCD writes.
- Debounce is a fixed delay on key press; key release is not debounced.
- The LCD busy flag is not read; fixed delays are used instead.
- Only one key press is handled at a time.

**Planned improvements**
- [ ] Replace software delays with SysTick-based timing
- [ ] Debounce both key press and release
- [ ] Read the LCD busy flag instead of using fixed delays
- [ ] Add chained operations to the state machine
- [ ] Expand the test table (large results, repeated `=`, operator after a result)
- [ ] Measure LCD write time before and after the timing changes

## Build
Built with Keil µVision. Startup files from NXP; core calculator logic and drivers are custom.
