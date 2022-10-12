Firefox Web Proxy
Written by: Ethan Huang

Features:
- Webpage Text: "Happy" to "Silly"
- Image: .jpg to one of two pictures [May not function, depending on whether or not the host of the image is active]

Compilation:
1. Move to the directory of "CProxy.cpp" which is as attached.
2. Run the following command to compile: "gcc CProxy.cpp -lpthread -o CProxy"
3. Run the program using the following: "./CProxy"

Configuration:
- If needed, change port via the #define PORT modifier at line 23
- If needed, change the IP of the Proxy via the address.sin_addr.s_addr( ... ) on line 38

How to Use (Firefox):
1. Enter Settings on the Firefox Browser
2. Find "Network Settings" in General
3. Under the above menu, a popup "Connection Settings" should appear
4. Click "Manual Proxy Configuration"
5. Under "HTTP Proxy" -- Enter the IP defined on address.sin_addr.s_addr( ... ) at line 38
	:: (Default is 2245)
6. Next to this entry, at "Port" -- Enter the Port number defined on line 23
	:: (Default is 136.159.5.27)
7. Once entered, click "Ok"
8. Enter some HTTP/ site, and the Proxy should be in effect.
9. To exit, you must enter CTRL+C on the terminal that's running said Proxy.

Notes:
- As mentioned on "How to Use (Firefox)" on Step 9, there wasn't a proper method used
	to exit the Proxy. In some concerns, I don't think this properly closes the socket
	opened from Client-Proxy, as the loop was yet to end. This could be modified to have
	some "Time Window" to browse, however, I didn't go through with it.
	At the moment, the Proxy continues to run until forcefully terminated via CTRL+C.
- This does utilize plenty of online resources, as cited throughout the code via comments.
	If there are any issues regarding this, please be sure to contact with me in any form.


