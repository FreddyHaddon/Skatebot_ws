# Message definition for the Theme 6 "Data Received" publisher node

# Identify which subsystem is the intended recipient of this data. It
# is possible for multiple subsystems to want the same data, but it
# will be up to them to perform a check on this field.
string dest_subsystem

# An identifier for who has sent the data. May or may not be used, but
# good to have.
string source_id

# Actual data. Represented as a byte array for maximum compatibility.
byte[] payload
