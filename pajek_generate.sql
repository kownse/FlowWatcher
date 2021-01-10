CREATE TABLE IF NOT EXISTS `pajek_tmp` (`row` int(11) NOT NULL AUTO_INCREMENT,`title` varchar(1000) NOT NULL, `wnd_id` int(11) NOT NULL,PRIMARY KEY (`row`)) ENGINE=MyISAM AUTO_INCREMENT=18 DEFAULT CHARSET=utf8;
CREATE TABLE IF NOT EXISTS `pajek_out` (`id` int(11) NOT NULL AUTO_INCREMENT,`from_wnd_id` int(11) NOT NULL,`to_wnd_id` int(11) NOT NULL DEFAULT '0',PRIMARY KEY (`id`)) ENGINE=MyISAM DEFAULT CHARSET=utf8;

truncate pajek_tmp;
insert into pajek_tmp(title, wnd_id) select init_title,wnd_id from rec_wnd where rec_id=379;
truncate pajek_out;
insert into pajek_out(from_wnd_id, to_wnd_id) select row, s.to_wnd_id from pajek_tmp as p, wnd_switch as s where s.from_wnd_id=p.wnd_id;
update pajek_out as p, pajek_tmp as t set p.to_wnd_id = t.row where t.wnd_id=p.to_wnd_id;

select row, title from pajek_tmp into outfile 'd:/pajek_title.txt' FIELDS TERMINATED BY " " LINES TERMINATED BY "\r\n";
select from_wnd_id, to_wnd_id from pajek_out into outfile 'd:/pajek_ed.txt' FIELDS TERMINATED BY " " LINES TERMINATED BY "\r\n";

DROP TABLE `pajek_tmp`;
DROP TABLE `pajek_out`;

