const HID = require("node-hid");

function start() {
  const devices = HID.devices();
  console.log({ devices });
  const deviceInfo = devices.find(
    (d) =>
      d.vendorId === 0xf055 &&
      d.productId === 0xa57a &&
      d.usagePage === 0xff00 &&
      d.usage === 0x01
  );
  console.log({ deviceInfo });
  if (deviceInfo) {
    const device = new HID.HID(deviceInfo.path);

    process.on("uncaughtException", (err) => {
      console.error(err);
      device.close();
      process.exit(1);
    });

    const onData = (data) => {
      const bytes = new Uint8Array(data);
      console.log("received");
      console.log({ bytes });
    };

    device.on("data", onData);

    setTimeout(async () => {
      console.log("sending...");
      const rawHidReportId = 4;
      device.write([rawHidReportId, 0x10, 0x11, 0x12, 0x13, 0x14, 0x15]);
    }, 1000);

    process.on("SIGINT", function () {
      device.close();
      process.exit(0);
    });
  }
}

start();
