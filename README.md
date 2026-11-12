# BareMetal-KV11-Calculator
A four-function integer calculator written in bare-metal C for the NXP Kinetis KV11 (MKV11Z7). Input comes from a 4x4 matrix keypad and results appear on an 8x2 Newhaven NHD-0208AZ character LCD. The LCD driver (4-bit mode) and keypad scanner are written from scratch using register-level GPIO access.

## Demo

<img width="685" height="909" alt="Screenshot 2026-11-07 173008" src="https://github.com/user-attachments/assets/b97a7aed-f5f0-41b5-9c1a-852e1f2a318d" />

## Hardware
| Part | Notes |
| :--- | :--- |
| **MCU** | NXP Kinetis KV11 (FRDM-KV11Z) |
| **Display** | 8x2 Newhaven NHD-0208AZ character LCD (4-bit mode, 3.3V logic) |
| **Input** | 4x4 matrix keypad |
| **Misc** | 10k potentiometer (LCD contrast), 2x 10Ω resistors (backlight current limiting) |

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
| RW | PTA1 (held low for write) |
| V0 (Contrast) | 10k potentiometer wiper |
| A (Backlight +) | 3.3V via 2x 10Ω resistors |

### Keypad
| Signal | MCU pin | Direction |
| :--- | :--- | :--- |
| Row 0-3 (driven) | PTC0-PTC3 | Output |
| Col 0-3 (read) | PTC4-PTC7 | Input (internal pull-up) |

## Operation

**Keypad Layout:**

```
* `1` `2` `3` `A` (add)
* `4` `5` `6` `B` (subtract)
* `7` `8` `9` `C` (clear)
* `*` `0` `#` `D` (divide)
```

`*` (multiply) `#` (equals)

## How it behaves

Numbers are capped at three digits for now so the full expression fits on the 8-character line. Operators and `=` do nothing until a number has been entered, and dividing by zero shows `ERROR`. Typing a digit after a result clears the screen and starts a new calculation.

## Architecture

* **LCD Driver:** The driver is written from scratch for 4-bit mode. Each byte is split into two nibbles and sent one after the other. RS selects whether a byte is a command or character data, RW is held low because the display is write-only, and a pulse on Enable latches each nibble in.
* **Keypad Scanner:** The scanner drives one row low at a time and reads all four columns. The columns have internal pull-ups, so they read high until a key is pressed. A press connects that column to the low row and pulls it low, and the row and column together index into a character map to get the key.
* **State Machine:** The calculator currently has three states. The first builds the first number. Once an operator is pressed, the second state builds the second number. Pressing `=` moves to the third state, which does the math, shows the result, and ignores input until the calculator is cleared.

## Testing

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
- Doesn't read the LCD busy flag, so it relies on fixed delays
- Only debounces on key press, not on key release
- Division is integer-only (the remainder is dropped)

**Planned improvements**
- Split code into separate modules (LCD, keypad, state machine) instead of one big file
- Wider operands (up to 7 digits): first value on line 1, operator and second value on line 2
- Answer shown on a cleared screen, starting top left and wrapping to line 2 if needed
- Split the state machine into four states: first value, operator, second value, result
- Interrupt-driven keypad handling instead of polling
- A 2nd function key that unlocks extra commands
- SysTick timing instead of busy-wait delays
- Chained operations
- Negative operands and a decimal point

## Build
Built with Keil µVision. Startup files from NXP; core calculator logic and drivers are custom.
