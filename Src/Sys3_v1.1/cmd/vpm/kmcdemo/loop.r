	array rtype[3];
	array xtype[3];
function loop()
	repeat {
		trace(0377);
		while (getxbuf(xtype) != 0);
		++nxbuf;
		trace(0376);
		while (getrbuf(rtype) != 0);
		trace(0375);
		++nrbuf;
		while(get(byte) == 0) 
			put(byte);
		rtnrbuf(rtype);
		rtnxbuf(xtype);
	}
end
