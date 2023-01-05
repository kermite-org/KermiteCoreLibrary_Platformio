const HID = require("node-hid");

function start() {
  const devices = HID.devices();
  console.log({ devices });
  const deviceInfo = devices.find(
    (d) =>
      d.vendorId === 0xf055 &&
      d.productId === 0xa57a &&
      d.usagePage === 65280 &&
      d.usage === 1
  );
  console.log({ deviceInfo });
  if (deviceInfo) {
    const device = new HID.HID(deviceInfo.path);

    const onData = (data) => {
      const bytes = new Uint8Array(data);
      console.log("received");
      console.log({ bytes });
    };

    device.on("data", onData);

    setTimeout(() => {
      console.log("sending...");
      // device.write([0xf2]);
      device.write([0, 0xf2]);
      // device.write([0x04, 0xf2]); //[0x04, 0x01, 0x01, 0x05, 0xff, 0xff]);
    }, 1000);

    process.on("SIGINT", function () {
      device.close();
      process.exit(0);
    });
  }
}

start();
