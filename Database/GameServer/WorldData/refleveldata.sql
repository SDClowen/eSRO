DROP TABLE IF EXISTS `refleveldata`;
CREATE TABLE  `refleveldata` (
	`level` int(3) NOT NULL,
	`exp` BIGINT(20) NOT NULL,
	`sp` int(11) NOT NULL,
	`base_xp` integer NOT NULL,
	`job_xp` integer NOT NULL
);
