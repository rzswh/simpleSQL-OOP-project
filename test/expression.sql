create database oop;
show databases;
use oop;
create TABLE qwe(wer INT, ert INT, rty INT, tyu INT NOT NULL, PRIMARY KEY(tyu));
INSERT INTO qwe(wer, ert, rty, tyu) VALUES (1, 2, 3, 4);
INSERT INTO qwe(wer, ert, rty, tyu) VALUES (1, 2, 3, 3);
INSERT INTO qwe(wer, ert, rty, tyu) VALUES (1, 2, 3, 2);
INSERT INTO qwe(wer, ert, rty, tyu) VALUES (1, 2, 3, 9);
INSERT INTO qwe(wer, ert, rty, tyu) VALUES (1, 2, 3, 10);
INSERT INTO qwe(wer, ert, rty, tyu) VALUES (1, 2, 3, 7);
INSERT INTO qwe(wer, ert, rty, tyu) VALUES (1, 2, 3, 8);
INSERT INTO qwe(wer, ert, rty, tyu) VALUES (1, 2, 3, 20);
INSERT INTO qwe(wer, ert, rty, tyu) VALUES (1, 2, 3, 30);
INSERT INTO qwe(wer, ert, rty, tyu) VALUES (1, 2, 3, 25);
UPDATE qwe SET wer=5 WHERE tyu>9;
UPDATE qwe SET ert=10 WHERE tyu<15;
select 2*(2*wer+2)=8 and tyu > 5, tyu, 8/0 from qwe;
select 6+6+6*(12+8-5/2)+1.2 from qwe;
