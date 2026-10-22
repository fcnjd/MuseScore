/*
 * SPDX-License-Identifier: GPL-3.0-only
 * MuseScore-Studio-CLA-applies
 *
 * MuseScore Studio
 * Music Composition & Notation
 *
 * Copyright (C) 2021 MuseScore Limited and others
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 3 as
 * published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */

pragma ComponentBehavior: Bound

import QtQuick
import QtQuick.Controls

import Muse.Ui
import Muse.UiComponents
import MuseScore.Braille

StyledFlickable {
    id: root
    enabled: brailleModel.enabled
    visible: enabled

    property NavigationPanel navigationPanel: NavigationPanel {
        name: "BrailleView"
        enabled: brailleTextArea.enabled && brailleTextArea.visible
        direction: NavigationPanel.Both
        accessible.name: qsTrc("braille/view", "Braille")
    }

    Component.onCompleted: {
        brailleModel.load()
        brailleTextArea.syncText()
    }

    BrailleModel {
        id: brailleModel
        property int keys_pressed: 0
        property string keys_buffer: ""

        onBrailleInfoChanged: brailleTextArea.syncText()
        onCurrentItemChanged: brailleTextArea.syncSelection()
    }

    TextArea.flickable: TextArea {
        id: brailleTextArea
        color: ui.theme.fontPrimaryColor
        font {
            pixelSize: ui.theme.bodyFont.pixelSize
        }
        readOnly: true
        selectByKeyboard: true
        wrapMode: Text.AlignLeft

		property bool synchronizing: false
		property bool navigating: false
		property var caretNavigationKeys: new Set()

		onActiveFocusChanged: {
			if (!activeFocus) {
				caretNavigationKeys.clear();
				brailleModel.keys_pressed = 0;
				brailleModel.keys_buffer = "";
			}
		}

		function syncText() {
			synchronizing = true;
			text = brailleModel.brailleInfo;
			synchronizing = false;
			syncSelection();
		}

		function syncSelection() {
			if (navigating || !focus || brailleModel.currentItemPositionStart < 0
					|| brailleModel.currentItemPositionEnd < 0) {
				return;
			}
			synchronizing = true;
			cursorPosition = Math.min(length, brailleModel.currentItemPositionEnd);
			synchronizing = false;
		}

		function isCaretNavigation(event) {
			return (event.key === Qt.Key_Left || event.key === Qt.Key_Right)
					&& event.modifiers === Qt.NoModifier;
		}

        function isInputMethodShortcut(event) {
            return event.key === Qt.Key_B
                   && event.modifiers === (Qt.ControlModifier | Qt.ShiftModifier)
        }

        property var keyMap: new Map([
            [Qt.Key_0, "0"],
            [Qt.Key_1, "1"],
            [Qt.Key_2, "2"],
            [Qt.Key_3, "3"],
            [Qt.Key_4, "4"],
            [Qt.Key_5, "5"],
            [Qt.Key_6, "6"],
            [Qt.Key_7, "7"],
            [Qt.Key_8, "8"],
            [Qt.Key_9, "9"],
            [Qt.Key_A, "A"],
            [Qt.Key_B, "B"],
            [Qt.Key_C, "C"],
            [Qt.Key_D, "D"],
            [Qt.Key_E, "E"],
            [Qt.Key_F, "F"],
            [Qt.Key_G, "G"],
            [Qt.Key_H, "H"],
            [Qt.Key_I, "I"],
            [Qt.Key_J, "J"],
            [Qt.Key_K, "K"],
            [Qt.Key_L, "L"],
            [Qt.Key_M, "M"],
            [Qt.Key_N, "N"],
            [Qt.Key_O, "O"],
            [Qt.Key_P, "P"],
            [Qt.Key_Q, "Q"],
            [Qt.Key_R, "R"],
            [Qt.Key_S, "S"],
            [Qt.Key_T, "T"],
            [Qt.Key_U, "U"],
            [Qt.Key_V, "V"],
            [Qt.Key_W, "W"],
            [Qt.Key_X, "X"],
            [Qt.Key_Y, "Y"],
            [Qt.Key_Z, "Z"],
            [Qt.Key_Space, "Space"],
            [Qt.Key_Right, "Right"],
            [Qt.Key_Left, "Left"],
            [Qt.Key_Up, "Up"],
            [Qt.Key_Down, "Down"],
            [Qt.Key_PageUp, "PageUp"],
            [Qt.Key_PageDown, "PageDown"],
            [Qt.Key_Home, "Home"],
            [Qt.Key_End, "End"],
            [Qt.Key_Delete, "Delete"],
            [Qt.Key_Escape, "Escape"],
            [Qt.Key_Minus, "Minus"],
            [Qt.Key_Plus, "Plus"],
         ])

        cursorDelegate: Rectangle {
            id: brailleCursor
            visible: brailleTextArea.cursorVisible
            color: ui.theme.isDark && brailleModel.cursorColor == "black" ? ui.theme.fontPrimaryColor : brailleModel.cursorColor
            width: brailleTextArea.cursorRectangle.width

            SequentialAnimation {
                loops: Animation.Infinite
                running: brailleTextArea.cursorVisible

                PropertyAction {
                    target: brailleCursor
                    property: 'visible'
                    value: true
                }

                PauseAnimation {
                    duration: 600
                }

                PropertyAction {
                    target: brailleCursor
                    property: 'visible'
                    value: false
                }

                PauseAnimation {
                    duration: 600
                }

                onStopped: {
                    brailleCursor.visible = false
                }
            }
        }

        NavigationControl {
            id: fakeNavCtrl
            name: "Braille"
            enabled: brailleTextArea.enabled && brailleTextArea.visible

            panel: root.navigationPanel
            order: 1

            accessible.role: MUAccessible.EditableText
            accessible.name: brailleModel.inputMethod === 0
                             ? (brailleModel.noteInputActive
                                //: Braille input with 6 keyboard keys (F,D,S & J,K,L) to represent the 6 dots in a braille cell.
                                ? qsTrc("braille/view", "Six-key input mode")
                                : qsTrc("braille/view", "Navigation mode, six-key selected"))
                             : (brailleModel.noteInputActive
                                ? qsTrc("braille/view", "MuseScore shortcuts mode")
                                : qsTrc("braille/view", "MuseScore shortcuts, note input off"))
            accessible.description: brailleModel.inputMethod === 0
                                    //: Short keyboard help for six-key input, available through the screen reader's control description command.
                                    ? qsTrc("braille/view", "Left and Right navigate Braille cells. Control+Shift+B switches input methods. N toggles note input. Tab or F6 moves focus.")
                                    //: Short keyboard help for MuseScore shortcuts input, available through the screen reader's control description command.
                                    : qsTrc("braille/view", "Left and Right navigate Braille cells. Control+Shift+B switches input methods. MuseScore shortcuts apply. Tab or F6 moves focus.")
            accessible.visualItem: brailleTextArea
            accessible.text: brailleTextArea.text
            accessible.selectedText: brailleTextArea.selectedText
            accessible.selectionStart: brailleTextArea.selectionStart
            accessible.selectionEnd: brailleTextArea.selectionEnd
            accessible.cursorPosition: brailleTextArea.cursorPosition

            onActiveChanged: {
                if (fakeNavCtrl.active) {
                    brailleTextArea.forceActiveFocus();
                    brailleTextArea.syncSelection();
                } else {
                    brailleTextArea.focus = false
                }
            }
        }

        NavigationFocusBorder {
            navigationCtrl: fakeNavCtrl
            drawOutsideParent: false
        }

        onCursorPositionChanged: {
			if (!synchronizing && !navigating && activeFocus) {
				// Selection notifications must not snap a user-driven caret to the item's end.
				navigating = true;
				brailleModel.cursorPosition = cursorPosition;
				navigating = false;
			}
        }

        Component.onCompleted: {
            shortcutOverrideModel.init()
        }

        ShortcutOverrideModel {
            id: shortcutOverrideModel
        }

        Keys.onShortcutOverride: function(event) {
			if (brailleTextArea.isCaretNavigation(event)) {
				// Plain arrows always navigate Braille cells, regardless of input method.
				event.accepted = true;
				return;
			}
            if (brailleTextArea.isInputMethodShortcut(event)) {
                // This panel-specific shortcut takes precedence over Insert B.
                event.accepted = true;
                return;
            }

            if (brailleModel.inputMethod !== 0 || !keyMap.has(event.key)) {
                // Let MuseScore resolve the user's current shortcut assignments.
                event.accepted = false;
                return;
            }

            // Six-key input owns these keys while this method is selected.
            event.accepted = true;
        }

        Keys.onPressed: function(event) {
			if (brailleTextArea.isCaretNavigation(event)) {
				caretNavigationKeys.add(event.key);
				// Let TextArea move its caret; these keys are not six-key chord input.
				event.accepted = false;
				return;
			}
            if (brailleTextArea.isInputMethodShortcut(event)) {
                if (!event.isAutoRepeat) {
                    brailleModel.toggleInputMethod();
                    brailleModel.keys_pressed = 0;
                    brailleModel.keys_buffer = "";
                }
                event.accepted = true;
                return;
            }

            if (event.key === Qt.Key_Tab
                    || event.key === Qt.Key_Backtab
                    || event.key === Qt.Key_F6
                    || event.key === Qt.Key_QuoteLeft) {
                //! NOTE: We need to handle Tab key here because https://doc.qt.io/qt-5/qml-qtquick-controls2-textarea.html#tab-focus
                //!       and we don't use qt navigation system
                if (shortcutOverrideModel.handleShortcut(event.key, event.modifiers)) {
                    brailleTextArea.focus = false;
                    event.accepted = true;
                    return;
                }
            }

			if (brailleModel.inputMethod !== 0) {
				// Unhandled keys remain available to MuseScore's shortcuts.
				event.accepted = false;
				return;
			}

            // Note: Subsequent keys must be accepted in Keys.onShortcutOverride (above).

            if (event.key !== Qt.Key_Shift
                && event.key !== Qt.Key_Alt
                && event.key !== Qt.Key_Control
            ) {

                ++brailleModel.keys_pressed;

                var keys = "";

                if (event.modifiers === Qt.ShiftModifier) {
                    keys = keys === "" ? "Shift" : keys += "+Shift";
                }

                if (event.modifiers === Qt.AltModifier) {
                    keys = keys === "" ? "Alt" : keys += "+Alt";
                }
                if (event.modifiers === Qt.ControlModifier) {
                    keys = keys === "" ? "Ctrl" : keys += "+Ctrl";
                }

                if (keys !== "") {
                    keys += "+";
                }

                if (keyMap.get(event.key) !== "") {
                    keys += keyMap.get(event.key);
                }

                if (keys !== "Left"
                    && keys !== "Right"
                    && keys !== "Up"
                    && keys !== "Down"
                ) {
                    if (brailleModel.keys_buffer !== "") {
                        brailleModel.keys_buffer += "+";
                    }
                    brailleModel.keys_buffer += keys;
                    event.accepted = true;
                }
            }
        }

        Keys.onReleased: function(event) {
			if (caretNavigationKeys.has(event.key)) {
				if (!event.isAutoRepeat) {
					caretNavigationKeys.delete(event.key);
				}
				event.accepted = true;
				return;
			}
            if (brailleTextArea.isInputMethodShortcut(event) || brailleModel.inputMethod !== 0) {
                event.accepted = false;
                return;
            }

            if (brailleModel.keys_pressed === 0) {
                return;
            }

            if (event.key !== Qt.Key_Shift
                && event.key !== Qt.Key_Alt
                && event.key !== Qt.Key_Control
            ) {
                brailleModel.keys_pressed--;

                if(brailleModel.keys_pressed <= 0) {
                    brailleModel.keys = brailleModel.keys_buffer;
                    brailleModel.keys_buffer = "";
                    brailleModel.keys_pressed = 0;
                    // set the focus back to braille
                    root.navigationPanel.setActive(true);
                    fakeNavCtrl.setActive(true);
                }
            }
        }
    }
}
