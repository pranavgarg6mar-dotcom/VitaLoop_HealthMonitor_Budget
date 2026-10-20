# CP210x Universal Windows Driver v11.6.0 — Installation Guide (Windows 11)

## Step 1: Download the Driver

Download the **CP210x Universal Windows Driver v11.6.0** from the
[Silicon Labs website](https://www.silabs.com/software-and-tools/usb-to-uart-bridge-vcp-drivers?tab=downloads).

> **Note:** This version fixes security vulnerabilities CVE-2026-15418 and CVE-2026-15419
> that affected v11.5.0 and earlier.

---

## Step 2: Extract the ZIP

1. Locate the downloaded ZIP file
2. Right-click on it and select **"Extract All..."**
3. Choose a destination folder and click **Extract**

> **Important:** Do NOT try to install directly from inside the ZIP archive.
> Windows will open the file in Notepad instead of installing it.

---

## Step 3: Install the Driver

1. Open the extracted folder
2. Find the `silabser` file with type **"Setup Information"** (14 KB)
3. **Right-click** on it and select **"Install"**
4. Click **"Yes"** if prompted by User Account Control (UAC)

---

## Folder Structure

After extraction, the folder should look like this:

```
CP210x_Universal_Windows_Driver/
├── arm/
├── arm64/
├── x64/
├── x86/
├── CP210x_Universal_Windows_Driver_ReleaseNotes.txt
├── silabser.cat       (Security Catalog)
├── silabser.inf       (Setup Information) ← Install this one
├── SLAB_License_Agreement_VCP_Windows.txt
├── UpdateParam.bat
└── UpdateParameters.reg
```

> **Note:** On Windows 11 x64, the installer will automatically use the
> correct driver files from the `x64` folder.

---

## Troubleshooting

If right-clicking does not show the **"Install"** option, use Device Manager instead:

1. Connect your CP210x device
2. Open **Device Manager**
3. Right-click on the device
4. Select **"Update driver"**
5. Point it to the extracted folder

---

## References

- [Silicon Labs CP210x Driver Downloads](https://www.silabs.com/software-and-tools/usb-to-uart-bridge-vcp-drivers?tab=downloads)
- [Silicon Labs Security Advisory A-00000586](https://github.com/SiliconLabsSoftware/docs-security-advisories/blob/main/Advisories/500-599/A-00000586.md)
