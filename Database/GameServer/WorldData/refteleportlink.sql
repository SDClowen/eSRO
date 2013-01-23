DROP TABLE IF EXISTS `refteleportlink`;
CREATE TABLE  `refteleportlink` (
  `srcid` int(11) NOT NULL,
  `destid` int(11) NOT NULL,
  `tax` int(11) NOT NULL,
  `minlv` int(11) NOT NULL,
  `maxlv` int(11) NOT NULL
);
