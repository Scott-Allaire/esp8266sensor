wifi.setmode(wifi.STATION)
wifi.sta.config("SSID","password")
dhtpin = 1;
adc.force_init_mode(adc.INIT_ADC);
srv=net.createServer(net.TCP)
srv:listen(80, function(conn)
    conn:on("receive", function(client, request)
        local buf = "";
        buf = buf.."HTTP/1.1 200 OK\r\n";
        buf = buf.."Content-Type: application/json\r\n";
        buf = buf.."\r\n"
        buf = buf.."{"
        local _, _, method, path, vars = string.find(request, "([A-Z]+) (.+)?(.+) HTTP");
        if(method == nil)then
            _, _, method, path = string.find(request, "([A-Z]+) (.+) HTTP");
        end
        -- buf = buf.."\"path\":\""..path.."\",";
        local status, temp, humi, temp_dec, humi_dec = dht.read(dhtpin);
        local luminance = adc.read(0)
        local temp_f = temp * 9 / 5 + 32;
        buf = buf.."\"temp\":\""..temp.."\",";
        buf = buf.."\"temp_f\":\""..temp_f.."\",";
        buf = buf.."\"humidity\":\""..humi.."\",";
        buf = buf.."\"luminance\":\""..luminance.."\"";
        buf = buf.."}\r\n"
        client:send(buf);
        client:close();
        collectgarbage();
    end)
end)
