import frida

def on_message(message, data):
    print("[on_message] message:", message, "data:", data)

pid = frida.spawn("main", ["main", "test.sqlite3", "bob"]) # Start the program, but paused
session = frida.attach(pid) # Hook into the program

script = session.create_script("""
rpc.exports.enumerateModules = () => {
  return Module.getExportByName("libsqlite3.so.0.8.6", "sqlite3_prepare_v2");
};
""")
script.on("message", on_message)
script.load()

print(script.exports_sync.enumerate_modules())

frida.resume(pid) # Run the program
