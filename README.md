# Project Description
This project was designed for home and apartment renters that want an affordable smart lock that is compliant with renter restrictions on changing locks (although anyone is welcome to use it). This is accomplished by utilizing existing lock hardware and keeping the keyed entry free for use. This design is compatible with most kinds of door locks and was made to be "plug and play." If modifications are needed, the source files included in this repository can be modified to fit your door. Assembly instructions are available through [instructables.com](https://www.instructables.com/) (project not yet released).

This smart lock uses an Arduino Nano as the microcontroller. It interfaces with a real time clock (the DS3231) to handle all scheduled events. The Nano also interfaces with a HC-05 Bluetooth module for users to program scheduled events, and manually engage or disengage the lock. The circuit is battery operated and uses a rechargeable 3.7v 18650 battery that is boosted to 5v. 

The smart lock is accessible/programmable via a bluetooth terminal smartphone app like the one **[here](https://play.google.com/store/apps/details?id=de.kai_morich.serial_bluetooth_terminal&hl=en_US&gl=US)**. Upon connecting and providing appropriate credentials, a menu will be displayed for the user allowing you to select several options. The major features are listed below:

## Modes:
Scheduling Mode: This option lets the user add, delete, or view existing schedules. When the desired schedules have been set the circuit enters into low power mode and can only be awoken by programmed interrupts on the DS3231 or the on board switch. This mode uses the least power at approximately 1 mAh. Using this mode gives the user the longest battery life with around 140 days before needing to be recharged.  

Bluetooth Mode: This mode uses the most power and is not recommended but is available for convenience if desired. This mode keeps the module paired, or open to pairing, so that the deadbolt can be locked or unlocked at any moment

Bluetooth Limited Mode: This mode combines the functionality of the previous two modes. The user defines a schedule for when the module should be on and pairable. This helps reduce unnecessary battery usage for time when the lock state is least likely to change (e.g. sleeping hours, working hours, school hours, etc.) 


## Quick Facts:
**Major Components:**
- Arduino Nano
- DS3231 Real Time Clock
- HC-05 Bluetooth Module 
- MG-90 Servo
- 18650 Battery
<!-- Total Project Time: ~40 hours -->
**Total project cost:** $25 (Market equivalents of this project cost ~$100+)  
See [Bill of Materials](#bill-of-materials)

**Battery Life**
140 day battery life in schedule mode°  
30 day battery life in Bluetooth limited mode*°  
All source code, 3D models, and instructions are available via a public Git repository  
*Based on a 3500 mAh 18650 battery
°Based on a 12 hour availability



# Bill of Materials
**NOTE:** Buying these components online usually means you'll need to purchase a pack or 2+. I'd recommend first looking for a local source if you don't have these components already. If a local supplier is unreasonable then [Temu](https://www.temu.com/) or [AliExpress](https://www.aliexpress.us/?gatewayAdapt=glo2usa&_randl_shipto=US) are your cheapsest options and [Amazon](https://www.amazon.com/) or [Ebay](https://www.ebay.com/) are your fastest options.
- [Arduino Nano](https://www.temu.com/type-c-usb-nano-3-0-with-bootloader-compatible-nano-3-0-controller-for-arduino-ch340-usb-driver-16mhz-original-ic-atmega328p-g-601099512651761.html?top_gallery_url=https%3A%2F%2Fimg.kwcdn.com%2Fproduct%2FFancyalgo%2FVirtualModelMatting%2F23b05b07fd59ddf50158f962c8f6c5fa.jpg&spec_gallery_id=7303184&refer_page_sn=10009&refer_source=10022&freesia_scene=2&_oak_freesia_scene=2&search_key=arduino%20nano&refer_page_el_sn=200049&_x_sessn_id=trkvj2oqle&refer_page_name=search_result&refer_page_id=10009_1678855981340_bgtll0qkhp)
- [HC-05 Bluetooth Module](https://www.ebay.com/itm/364130886349?hash=item54c7e462cd:g:~b4AAOSwv9JiSd28&amdata=enc%3AAQAHAAAA0NYbodF2PNOmemT6Iv%2Ft161a1K9huuwFcAhhemOSBLoI4UES%2BxQcD%2BK8EUdHrdTKE5XuTzksNRReq%2FEOTlGRsyNsLuh8tSmazfkKLyGRTIudKyNJ1%2BD6eNAd1Hq0%2BA0HW6WoP2qgvQV3s8E7hYSU7uMbeNaAuZ3G0cH3odQxOWtDw%2FNNGNM4fnvepMkZg0UsXSm%2FQthBSyg379MQ8%2FYOxy3wEvy54wdiggH7X8QjBk5F%2FauTNWP3J5OFJHwEcKXyxf0me0UjaEtKn9Ebf8XPjEk%3D%7Ctkp%3ABFBM-tL1udxh)
- [DS3231 Real Time Clock](https://www.amazon.com/DS3231-AT24C32-Module-Memory-Replace/dp/B09LLMYBM1/ref=sr_1_3?crid=1BLEU9LECRJMZ&keywords=DS3231&qid=1678856149&sprefix=ds3231%2Caps%2C210&sr=8-3)
- [Portable Battery Pack](https://www.ebay.com/itm/255619278468?_trkparms=amclksrc%3DITM%26aid%3D1110006%26algo%3DHOMESPLICE.SIM%26ao%3D1%26asc%3D20201210111314%26meid%3D1b708009ddcf41b89690278c51b97c9c%26pid%3D101195%26rk%3D3%26rkt%3D12%26sd%3D333142578258%26itm%3D255619278468%26pmt%3D1%26noa%3D0%26pg%3D2047675%26algv%3DSimplAMLv11WebTrimmedV3MskuWithLambda85KnnRecallV1V4V6ItemNrtInQueryAndCassiniVisualRankerAndBertRecall%26brand%3DUnbranded&_trksid=p2047675.c101195.m1851&amdata=cksum%3A2556192784681b708009ddcf41b89690278c51b97c9c%7Cenc%3AAQAHAAABUBEC%252B8HimpTF6XPanG8lCeF3G6jifrewDlLoKJMPz0RnJH8s%252BUBlIo2cSQSdgSGvSVvCKC6zF4X6tOnQIr392G0gldH8QjhGQ2fmdEokLVFt29UKj%252FEph29NUO0KxqhEb84gZhbKWlWfbravBJF8o5LISgG2dcpA51F2tp%252FAjdDXVPbsVnN4DaGk68mYhZIyZfhGTL6kZyQcEA0gVIBNUF40sRI1vM4krT9djMNQBgYgGtmT09IXDkW3pugX7k9pXznTdOzG3awVKD14ZHS06%252BuF0NdSErPNxEIyY%252F6%252B0FhdfccID7hNfi%252Bo8W3D9sxwQxg3z5HnpjgFuwt73ec%252BuI5udWyjeuvYg%252FisPPivejm0U%252BSObok3xOkZNrQkze1ZFE6lQKMSdZNB8XRJfNykS2PQ7Bk%252FAXMipgmR25zL7I%252FCNeJ90RU6IMPzBVHOa44hOQ%253D%253D%7Campid%3APL_CLK%7Cclp%3A2047675)** 
- [MG90 Servo](https://www.temu.com/mg90s-metal-gear-digital-9g-servo-for-rc-helicopter-plane-boat-car-mg90-9g-g-601099512694957.html?top_gallery_url=https%3A%2F%2Fimg.kwcdn.com%2Fproduct%2FFancyalgo%2FVirtualModelMatting%2F4a2390a98d1a7c1fa8c26db3e75cd98e.jpg&spec_gallery_id=7352047&refer_page_sn=10009&refer_source=10022&freesia_scene=2&_oak_freesia_scene=2&search_key=mg90s&refer_page_el_sn=200049&_x_sessn_id=soctzd02zb&refer_page_name=search_result&refer_page_id=10009_1678856797092_qg7j44zg6v)  
**If you can afford it, I'd recommend spending a bit more money to purchase a nice battery. Many cheap batteries on the internet lie about their capacity and you'll severally limit the battery life of the smart lock if you get a bad battery - could just buy a [case](https://www.aliexpress.us/item/3256802714206963.html?spm=a2g0o.productlist.main.3.b276YyLwYyLwDm&algo_pvid=d92820b4-48ba-4548-8858-86d2e304d7d0&algo_exp_id=d92820b4-48ba-4548-8858-86d2e304d7d0-1&pdp_ext_f=%7B%22sku_id%22%3A%2212000022681529511%22%7D&pdp_npi=3%40dis%21USD%215.69%212.84%21%21%21%21%21%40212249cb16788561791555458d0706%2112000022681529511%21sea%21US%210&curPageLogUid=1C2PYCcguKhi) (An 18650 battery case if important because it has a built in charing circuit and boost converter) and then get the [battery](https://www.18650batterystore.com/products/samsung-35e-protected) separtely to ensure you get a good product
- 2x MOSFET
- SPST Switch
- Momentary Switch
- 2x 10k ohm resistors
- 1x 2k ohm resistor
- 1x 4k ohm resistor
- 2x 330 microFarad Capacitor
- PCB - see pcb under [Systems](#systems)


# Instructions
Work in progress. Will be posted on instructables.com

# Systems
## Code  
## PCB  
## Mechanical

# Acknowledgements
Special thanks to the following individuals for providing critical support and feedback to make this project a success:  
Jarom Christensen   -   PCB design  
Devin Bingham       -   PCB debugging  
Michael Davis       -   Feature suggestions and market valuation  
My Wife             -   Letting me use our kitchen table as my workbench for 3 weeks  
