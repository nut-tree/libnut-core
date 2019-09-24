/* jshint esversion: 6 */
const libnut = require("../..");
const targetpractice = require("targetpractice/index.js");

libnut.setMouseDelay(100);

let target, elements;
let originalTimeout;

describe("Integration/Keyboard", () => {
  beforeEach(done => {
    originalTimeout = jasmine.DEFAULT_TIMEOUT_INTERVAL;
    jasmine.DEFAULT_TIMEOUT_INTERVAL = 15000;

    target = targetpractice.start();
    target.once("elements", message => {
      elements = message;
      console.log(elements);
      done();
    });
  });

  afterEach(() => {
    jasmine.DEFAULT_TIMEOUT_INTERVAL = originalTimeout;
    targetpractice.stop();
    target = null;
  });

  it("types", done => {
    const stringToType = "hello world";
    // Currently Target Practice waits for the "user" to finish typing before sending the event.
    target.once("type", element => {
      expect(element.id).toEqual("input_1");
      expect(element.text).toEqual(stringToType);
      done();
    });

    const input_1 = elements.input_1;
    libnut.moveMouse(input_1.x, input_1.y);
    libnut.mouseClick();
    libnut.typeString(stringToType);
  });
});
