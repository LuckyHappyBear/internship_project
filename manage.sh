#!/bin/bash

command=$1
DEV=$2
PORT=$3

start_routing()
{
    #1.add a root queue and the package has no class pass this queue
    tc qdisc add dev $DEV root handle 1:0 htb

    #1.1 add a main class which class id is 1: rate is $UPLINK kbits
    tc class add dev $DEV parent 1:0 classid 1:1 htb rate 1000kbit ceil 1000kbit prio 0

    #1.1.1 add the first leaf class under main class, this class has highest priority
    tc class add dev $DEV parent 1:1 classid 1:11 htb rate 1000kbit ceil 1000kbit prio 1

    #1.1.2 add the second leaf class under main class, this class has second priority
    tc class add dev $DEV parent 1:1 classid 1:12 htb rate 500kbit ceil 750kbit prio 2

    #1.2 add the proximate grade class which class id is 1:2,this class rate is lower than main class
    tc class add dev $DEV parent 1:0 classid 1:2 htb rate 500kbit prio 3

    #1.2.1 add the first leaf class under the proximate grade class
    tc class add dev $DEV parent 1:2 classid 1:21 htb rate 400kbit ceil 500kbit prio 4

    #1.2.2 add the second leaf class under the proximate grade class
    tc class add dev $DEV parent 1:2 classid 1:22 htb rate 300kbit ceil 400kbit prio 5

    #1.2.3 add the third leaf class under the proximate grade class
    tc class add dev $DEV parent 1:2 classid 1:23 htb rate 200kbit ceil 300kbit prio 6

    #1.2.4 add the fourth leaf class under the proximate grade class
    tc class add dev $DEV parent 1:2 classid 1:24 htb rate 10kbit ceil 200kbit prio 7

    #add another queue regulation sfq,avoid a connection use queue for a long time
    tc qdisc add dev $DEV parent 1:11 handle 111: sfq perturb 5
    tc qdisc add dev $DEV parent 1:12 handle 112: sfq perturb 5
    tc qdisc add dev $DEV parent 1:21 handle 121: sfq perturb 10
    tc qdisc add dev $DEV parent 1:22 handle 122: sfq perturb 10
    tc qdisc add dev $DEV parent 1:23 handle 133: sfq perturb 10
    tc qdisc add dev $DEV parent 1:24 handle 124: sfq perturb 10

    #set filter here
    tc filter add dev $DEV parent 1:0 protocol ip prio 1 handle 1 fw classid 1:11
    tc filter add dev $DEV parent 1:0 protocol ip prio 2 handle 2 fw classid 1:12
    tc filter add dev $DEV parent 1:0 protocol ip prio 3 handle 3 fw classid 1:21
    tc filter add dev $DEV parent 1:0 protocol ip prio 4 handle 4 fw classid 1:22
    tc filter add dev $DEV parent 1:0 protocol ip prio 5 handle 5 fw classid 1:23
    tc filter add dev $DEV parent 1:0 protocol ip prio 6 handle 6 fw classid 1:24

    exit 1
}

start_mangle()
{
    #mark package from 1-6 according to dport,let them pass different path
    #give return statement to accelarate process speed

    #set rule according to TOS
    iptables -t mangle -A PREROUTING -m tos --tos Minimize-Delay -j MARK --set-mark 1
    iptables -t mangle -A PREROUTING -m tos --tos Minimize-Delay -j RETURN
    iptables -t mangle -A PREROUTING -m tos --tos Minimize-Cost -j MARK --set-mark 4
    iptables -t mangle -A PREROUTING -m tos --tos Minimize-Cost -j RETURN
    iptables -t mangle -A PREROUTING -m tos --tos Maximize-Throughput -j MARK --set-mark 5
    iptables -t mangle -A PREROUTING -m tos --tos Maximize-Throughput -j RETURN

    #give tcp initial connection data package higher priority
    iptables -t mangle -A PREROUTING -p tcp -m tcp --tcp-flags SYN,RST,ACK SYN -j MARK --set-mark 1
    iptables -t mangle -A PREROUTING -p tcp -m tcp --tcp-flags SYN,RST,ACK SYN -j RETURN

    #small packages pass first
    iptables -t mangle -A PREROUTING -p tcp -m length --length :64 -j MARK --set-mark 2
    iptables -t mangle -A PREROUTING -p tcp -m length --length :64 -j RETURN

    #set ftp in queue 2, set ftp-data in queue 5
    iptables -t mangle -A PREROUTING -p tcp -m tcp --dport ftp -j MARK --set-mark 2
    iptables -t mangle -A PREROUTING -p tcp -m tcp --dport ftp -j RETURN
    iptables -t mangle -A PREROUTING -p tcp -m tcp --dport ftp-data -j MARK --set-mark 5
    iptables -t mangle -A PREROUTING -p tcp -m tcp --dport ftp-data -j RETURN

    #set http and https in queue 4
    iptables -t mangle -A PREROUTING -p tcp -m tcp --dport 80 -j MARK --set-mark 4
    iptables -t mangle -A PREROUTING -p tcp -m tcp --dport 80 -j RETURN

    iptables -t mangle -A PREROUTING -p tcp -m tcp --dport 443 -j MARK --set-mark 4
    iptables -t mangle -A PREROUTING -p tcp -m tcp --dport 443 -j RETURN

    #voip go first
    iptables -t mangle -A PREROUTING -p tcp -m tcp --dport 1720 -j MARK --set-mark 1
    iptables -t mangle -A PREROUTING -p tcp -m tcp --dport 1720 -j RETURN
    iptables -t mangle -A PREROUTING -p udp -m udp --dport 1720 -j MARK --set-mark 1
    iptables -t mangle -A PREROUTING -p udp -m udp --dport 1720 -j RETURN

    #finish prerouting table use this rule
    iptables -t mangle -A PREROUTING -i $DEV -j MARK --set-mark 6
    exit 1
}

add_mangle()
{
    iptables -t mangle -A PREROUTING -p tcp -m tcp --sport $PORT -j MARK --set-mark 5
}

stop_mangle()
{
    iptables -t mangle -F
}


stop_routing()
{
    tc qdisc del dev $DEV root
}

show_tc_qdisc()
{
    tc qdisc show dev $DEV
}

show_tc_class()
{
    tc class show dev $DEV
}

show_tc_fliter()
{
    tc filter show dev $DEV
}

show_mangle()
{
    iptables -n -L -t mangle
}

case $command in
    (start_routing)
        start_routing
        ;;
    (start_mangle)
        start_mangle
        ;;
    (stop_mangle)
        stop_mangle
        ;;
    (stop_routing)
        stop_routing
        ;;
    (show_tc_qdisc)
        show_tc_qdisc
        ;;
    (show_tc_class)
        show_tc_class
        ;;
    (show_tc_fliter)
        show_tc_fliter
        ;;
    (show_mangle)
        show_mangle
        ;;
    (add_mangle)
        add_mangle
        ;;
esac
