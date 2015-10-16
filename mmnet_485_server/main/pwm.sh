
if false; then

wget -q -O /dev/null http://192.168.88.128/cgi-bin/item.cgi?name=pwm0\&value=170
wget -q -O /dev/null http://192.168.88.128/cgi-bin/item.cgi?name=pwm1\&value=170
wget -q -O /dev/null http://192.168.88.128/cgi-bin/item.cgi?name=pwm2\&value=170
wget -q -O /dev/null http://192.168.88.128/cgi-bin/item.cgi?name=pwm3\&value=170

else

wget -q -O /dev/null http://192.168.88.128/cgi-bin/item.cgi?name=pwm0\&value=254
wget -q -O /dev/null http://192.168.88.128/cgi-bin/item.cgi?name=pwm1\&value=254
wget -q -O /dev/null http://192.168.88.128/cgi-bin/item.cgi?name=pwm2\&value=254
wget -q -O /dev/null http://192.168.88.128/cgi-bin/item.cgi?name=pwm3\&value=254

fi