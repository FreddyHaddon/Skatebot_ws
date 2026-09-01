#!/bin/bash
# Install and setup IMU pre-requsites.

# Stop on first error.
set -e

# Tell the user what is going on.
echo
echo "Running $0..."
echo

# Get the directory this script is in.
install_dir="$( cd "$( dirname "${BASH_SOURCE[0]}" )" &>/dev/null && pwd )"


# Only install once.
if [ ! -e ~/git/rpi4_adafruit_mic_speaker ]
then
    sudo apt update
    sudo apt upgrade -y
    sudo apt install -y dkms linux-libc-dev
    sudo pip3 install --upgrade adafruit-python-shell
    
    cd ~/git
    git clone https://github.com/pipebots/rpi4_adafruit_mic_speaker.git
    cd ~/git/rpi4_adafruit_mic_speaker
    sudo python3 i2smic.py

    sudo cp ${install_dir}/asound.conf /etc/asound.conf
    cat ${install_dir}/audio_config.txt | sudo tee -a /boot/firmware/config.txt
    sudo sed -i 's|dtparam=audio=on|# dtparam=audio=on|g' /boot/firmware/config.txt 
fi

echo
echo "IMPORTANT"
echo "Please reboot for the changes to take effect."
echo
echo "Test microphone:"
echo "$ arecord -l"
echo  "card 1: sndrpii2scard [snd_rpi_i2s_card], device 0: simple-card_codec_link snd-soc-dummy-dai-0 [simple-card_codec_link snd-soc-dummy-dai-0]"
echo "Subdevices: 1/1"
echo   "Subdevice #0: subdevice #0"
echo  "Then it works!"
echo
echo "Test speaker using:"
echo "$ arecord -l"
echo  "card 1: sndrpii2scard [snd_rpi_i2s_card], device 0: simple-card_codec_link snd-soc-dummy-dai-0 [simple-card_codec_link snd-soc-dummy-dai-0]"
echo "Subdevices: 1/1"
echo   "Subdevice #0: subdevice #0"
echo  "Then it works!"
echo
echo "$0 took $SECONDS seconds."
echo
