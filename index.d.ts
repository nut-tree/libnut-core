export interface Bitmap {
  width: number;
  height: number;
  image: any;
  byteWidth: number;
  bitsPerPixel: number;
  bytesPerPixel: number;
}

export interface Screen {
  capture(x?: number, y?: number, width?: number, height?: number): Bitmap;
  highlight(x: number, y: number, width: number, height: number, duration: number, opacity: number): void;
}

export interface Point {
  x: number;
  y: number
}

export interface Size {
  width: number;
  height: number;
}

export interface Rect {
  x: number;
  y: number;
  width: number;
  height: number;
}

export function setKeyboardDelay(ms: number): void;
export function keyTap(key: string, modifier?: string | string[]): void;
export function keyToggle(
  key: string,
  down: string,
  modifier?: string | string[]
): void;
export function typeString(string: string): void;
export function typeStringDelayed(string: string, cpm: number): void;
export function setMouseDelay(delay: number): void;
export function moveMouse(x: number, y: number): void;
export function moveMouseSmooth(x: number, y: number): void;
export function mouseClick(button?: string, double?: boolean): void;
export function mouseToggle(down?: string, button?: string): void;
export function dragMouse(x: number, y: number): void;
export function scrollMouse(x: number, y: number): void;
export function getMousePos(): Point;
export function getScreenSize(): Size;
export function getWindows(): number[];
export function getActiveWindow(): number;
export function getWindowRect(handle: number): Rect;
export function getWindowTitle(handle: number): string;

/**
 * Sets the focus to a specific window using its handle.
 *
 * @param {number} handle - The handle ID of the window to be focused.
 * @returns {void}
 */
export function focusWindow(handle: number): void

/**
* Resizes a window by its handle to the given width and height.
* The window is moved to the x & y coordinates if specified.
*
* @param {number} handle - The handle ID of the window to be resized.
* @param {Rect} rect - The new size of the window.
* @param {number} rect.x - The new x coordinate of the window.
* @param {number} rect.y - The new y coordinate of the window.
* @param {number} rect.width - The new width of the window.
* @param {number} rect.height - The new height of the window.
* @returns {void}
*/
export function resizeWindow(handle: number, rect: Rect): void 

export const screen: Screen;
