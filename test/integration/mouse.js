/* jshint esversion: 6 */
const libnut = require("../..");
const targetpractice = require("targetpractice/index.js");
const os = require("os");

libnut.setMouseDelay(100);

let target, elements;
let originalTimeout;

describe("Integration/Mouse", () => {
  beforeEach(done => {
    originalTimeout = jasmine.DEFAULT_TIMEOUT_INTERVAL;
    jasmine.DEFAULT_TIMEOUT_INTERVAL = 15000;

    target = targetpractice.start();
    target.once("elements", message => {
      elements = message;
      done();
    });
  });

  afterEach(() => {
    jasmine.DEFAULT_TIMEOUT_INTERVAL = originalTimeout;
    targetpractice.stop();
    target = null;
  });

  it("clicks", done => {
    // Alright we got a click event, did we click the button we wanted?
    target.once("click", function(e) {
      expect(e.id).toEqual("button_1");
      expect(e.type).toEqual("click");
      done();
    });

    // For this test we want a button.
    const button_1 = elements.button_1;
    // Click it!
    libnut.moveMouse(button_1.x, button_1.y);
    libnut.mouseClick();
  });

  it("scrolls vertically", done => {
    target.once("scroll", element => {
      /**
       *  TODO: This is gross! The scroll distance is different for each OS. I want
       *  to look into this further, but at least these numbers are consistent.
       */
      let expectedScroll;
      switch (os.platform()) {
        case "linux":
          expectedScroll = 180;
          break;
        case "win32":
          expectedScroll = 8;
          break;
        default:
          expectedScroll = 10;
      }
      expect(element.id).toEqual("textarea_1");
      expect(element.scroll_y).toEqual(expectedScroll);
      done();
    });

    const textarea_1 = elements.textarea_1;
    libnut.moveMouse(textarea_1.x, textarea_1.y);
    libnut.mouseClick();
    libnut.scrollMouse(0, -10);
  });

  it("scrolls horizontally", done => {
    target.once("scroll", element => {
      /**
       *  TODO: This is gross! The scroll distance is different for each OS. I want
       *  to look into this further, but at least these numbers are consistent.
       */
      let expectedScroll;
      switch (os.platform()) {
        case "linux":
          expectedScroll = 530;
          break;
        case "win32":
          expectedScroll = 8;
          break;
        default:
          expectedScroll = 10;
      }
      expect(element.id).toEqual("textarea_1");
      expect(element.scroll_x).toEqual(expectedScroll);
      done();
    });

    const textarea_1 = elements.textarea_1;
    libnut.moveMouse(textarea_1.x, textarea_1.y);
    libnut.mouseClick();
    libnut.scrollMouse(-10, 0);
  });
});
