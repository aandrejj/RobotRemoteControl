// Declare the call back functionat_cmd
void toggleBacklight(uint16_t isOn);
void hide_menu();
void back_menu();
void start_BT_pair();
void show_measured_data();

void at_cmd();
void at_disc_cmd();
void at_addr_cmd();
void at_connl_cmd();
void at_con_adr1_cmd();
void at_con_adr2_cmd();
void at_name_cmd();

extern MenuItem* settingsMenu[];
extern MenuItem* bluetoothMenu[];

// Initialize the main menu items
MAIN_MENU(
    ITEM_COMMAND("Show measured data", show_measured_data),
    ITEM_COMMAND("dexHand 3CA308B4E3B5", at_con_adr1_cmd),
    ITEM_COMMAND("gripHand0CB2B766319C", at_con_adr2_cmd),
    ITEM_SUBMENU("Bluetooth", bluetoothMenu),
    ITEM_SUBMENU("Settings", settingsMenu),
    ITEM_TOGGLE("Backlight", toggleBacklight),
    ITEM_COMMAND("Hide Menu", hide_menu)
);
    //ITEM_COMMAND("Connect to last BT", start_BT_pair),

/**
 * Create submenu and precise its parent
 */
SUB_MENU(settingsMenu, mainMenu,
    ITEM_BASIC("Backlight"),
    ITEM_BASIC("Contrast"),
    ITEM_COMMAND("..Back", back_menu)
);

SUB_MENU(bluetoothMenu, mainMenu,
    ITEM_COMMAND("AT", at_cmd),
    ITEM_COMMAND("AT+NAME?", at_name_cmd),
    ITEM_COMMAND("AT+DISC?", at_disc_cmd),
    ITEM_COMMAND("AT+ADDR?", at_addr_cmd),
    ITEM_COMMAND("AT+CONNL", at_connl_cmd),
    ITEM_COMMAND("dexHand  AT+CON3CA308B4E3B5", at_con_adr1_cmd),
    ITEM_COMMAND("gripHand AT+CON0CB2B766319C", at_con_adr2_cmd),
    ITEM_COMMAND("..Back", back_menu)
);