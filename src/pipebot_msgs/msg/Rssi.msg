# Message definition for the Theme 6 "RSSI" publisher node

# Identify which communication network the Pipebot is connected to
# Currently coded as a <string> but might be changed to an <uint8>
string network_id

# The value of the RSSI metric can be either positive or negative
float32 rssi_value

# There are multiple ways of reporting the RSSI value, therefore the
# units are important.
# Currently coded as a <string> but might be changed to an <uint8>
string rssi_units
