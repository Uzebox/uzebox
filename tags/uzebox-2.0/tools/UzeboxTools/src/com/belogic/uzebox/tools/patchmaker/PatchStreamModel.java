/*
    Uzebox toolset
    Copyright (C) 2008  Alec Bourque

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.    
 */
package com.belogic.uzebox.tools.patchmaker;

import javax.swing.table.AbstractTableModel;

public class PatchStreamModel extends AbstractTableModel {
	private static final long serialVersionUID = 1L;
	private String[] colNames = {"Step", "Delta Time", "Command", "Parameter" };
	
	public PatchStreamModel() {

	}

	@Override
	public int getColumnCount() {
		return colNames.length;
	}

	@Override
	public int getRowCount() {
		return 0;
	}

	@Override
	public Object getValueAt(int arg0, int arg1) {
		return null;
	}

	@Override
    public String getColumnName(int col) {
		return colNames[col];
    }
	
	@Override
	public Class getColumnClass(int c) {
		return getValueAt(0, c).getClass();
	}
	
}
