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

export const screen: Screen;
