const HID = require("node-hid");
const devices = HID.devices();
const deviceInfo = devices.find(
  (d) => d.vendorId === 0xf055 && d.productId === 0xa57a
);
if (deviceInfo) {
  const device = new HID.HID(deviceInfo.path);

  const onData = (data) => {
    const bytes = new Uint8Array(data);
    console.log("received");
    console.log({ bytes });
  };

  device.on("data", onData);

  console.log("sending...");
  device.write([0x00, 0x01, 0x01, 0x05, 0xff, 0xff]);

  process.on("SIGINT", function () {
    device.close();
    process.exit(0);
  });
}
