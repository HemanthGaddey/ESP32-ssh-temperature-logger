#!/bin/sh

latest_log=$(head -n 1 /home/gagan/TemperatureLogs/TemperaturerLog)

temp=$(echo ${latest_log} | awk '{print $3}')
timestamp_latest_log=$(echo ${latest_log} | awk '{print $1, $2}')
seconds_latest_log=$(date -d "${timestamp_latest_log}" +"%s")

last_email_1=$(sed -n 1p lastTime) #for inactive ESP32
last_email_2=$(sed -n 2p lastTime) #for temperature alert
timestamp_le1=$(echo ${last_email_1} | awk '{print $1, $2}')
timestamp_le2=$(echo ${last_email_2} | awk '{print $1, $2}')
seconds_le1=$(date -d "${timestamp_le1}" +"%s")
seconds_le2=$(date -d "${timestamp_le2}" +"%s")

seconds_now=$(date +"%s")

time_since_last_log=$(($seconds_now - $seconds_latest_log))
time_since_email_1=$(($seconds_now - $seconds_le1))
time_since_email_2=$(($seconds_now - $seconds_le2))

#If Esp32 stopped logging temperature more than 3 minutes ago and last error mail was sent more than an hour ago:
if [ $time_since_last_log -gt 180 ] && [ $time_since_email_1 -gt 3600 ]
then
    echo "Temperature Logging has stopped"  >> test
    echo "Esp32 has unfortunately stopped logging temperature data. Please restart the Esp32 by pressing the 'rst' button on it under stable wifi and please make sure wires are't connected loosely." | mutt -s "Temperature Logging Error" gaddeyhemanth@iitbhilai.ac.in
    sed -i '1s/.*/$(date +"%D %T")/' /home/gagan/TemperatureLogs/lastTime #update email sent time
fi

#If Temperature is >=30'C and Last Email about this was sent more than an hour ago:
if [ $time_since_email_2 -gt 3600 ] && (( $(echo "$temp >= 30.0" |bc -l) ))
then
    echo "Time elapsed since the first line: $elapsed_minutes minutes. Sending email..." >> test
    echo "The temperature of server is currently $(temp) degree Centigrade." | mutt -s "Alert! Server Temperature is crossing 30C" gaddeyhemanth@iitbhilai.ac.in
    sed -i '2s/.*/$(date +"%D %T")/' /home/gagan/TemperatureLogs/lastTime  #update email sent time
fi
