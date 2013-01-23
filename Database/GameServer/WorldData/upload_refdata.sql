delete from refpackageitem;
delete from refpricepolicyofitem;
delete from refshopgoods;
delete from refshopgroup;
delete from refshoptab;

load data infile '/home/npcdoom/PK2Tools/Media/server_dep/Silkroad/conv/refpackageitem.txt' into table refpackageitem character set utf8;
load data infile '/home/npcdoom/PK2Tools/Media/server_dep/Silkroad/conv/refpricepolicyofitem.txt' into table refpricepolicyofitem character set utf8;
load data infile '/home/npcdoom/PK2Tools/Media/server_dep/Silkroad/conv/refshopgoods.txt' into table refshopgoods character set utf8;
load data infile '/home/npcdoom/PK2Tools/Media/server_dep/Silkroad/conv/refshopgroup.txt' into table refshopgroup character set utf8;
load data infile '/home/npcdoom/PK2Tools/Media/server_dep/Silkroad/conv/refshoptab.txt' into table refshoptab character set utf8;
