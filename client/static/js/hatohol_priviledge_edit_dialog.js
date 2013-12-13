/*
 * Copyright (C) 2013 Project Hatohol
 *
 * This file is part of Hatohol.
 *
 * Hatohol is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 2 of the License, or
 * (at your option) any later version.
 *
 * Hatohol is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with Hatohol. If not, see <http://www.gnu.org/licenses/>.
 */


var HatoholPriviledgeEditDialog = function(applyCallback) {
  var self = this;
  self.mainTableId = "priviledgeEditDialogMainTable";
  self.applyCallback = applyCallback;
  self.serversData = null;

  var dialogButtons = [{
    text: gettext("APPLY"),
    click: function() { self.applyButtonClicked(); }
  }, {
    text: gettext("CANCEL"),
    click: function() { self.cancelButtonClicked(); }
  }];

  // call the constructor of the super class
  HatoholDialog.apply(
    this, ["priviledge-edit-dialog", "Edit priviledges", dialogButtons]);
  self.start();
};

HatoholPriviledgeEditDialog.prototype =
  Object.create(HatoholDialog.prototype);
HatoholPriviledgeEditDialog.prototype.constructor = HatoholPriviledgeEditDialog;

HatoholPriviledgeEditDialog.prototype.createMainElement = function() {
  var ptag = $("<p/>");
  ptag.attr("id", "priviledgeEditDialogMsgArea");
  ptag.text(gettext("Now getting information..."));
  return ptag;
};

HatoholPriviledgeEditDialog.prototype.applyButtonClicked = function() {
  if (!this.applyCallback)
    return;
  this.applyCallback();
  this.closeDialog();
};

HatoholPriviledgeEditDialog.prototype.cancelButtonClicked = function() {
  this.closeDialog();
};

HatoholPriviledgeEditDialog.prototype.setMessage = function(msg) {
  $("#priviledgeEditDialogMsgArea").text(msg);
};

HatoholPriviledgeEditDialog.prototype.start = function() {
  var self = this;
  $.ajax({
    url: "/tunnel/server",
    type: "GET",
    data: {},
    success: function(serversData) {
      var replyParser = new HatoholReplyParser(serversData);
      if (replyParser.getStatus() !== REPLY_STATUS.OK) {
        self.setMessage(replyParser.getStatusMessage());
        return;
      }
      if (!serversData.numberOfServers) {
        self.setMessage(gettext("No data."));
        return;
      }

      self.serversData = serversData;
      self.updateServersTable();
    },
    error: function(XMLHttpRequest, textStatus, errorThrown) {
      var errorMsg = "Error: " + XMLHttpRequest.status + ": " +
                     XMLHttpRequest.statusText;
      self.setMessage(errorMsg);
    }
  });
};

HatoholPriviledgeEditDialog.prototype.updateServersTable = function() {
  if (!this.serversData)
    return;

  var table = this.generateMainTable();
  var rows = this.generateTableRows(this.serversData);
  this.replaceMainElement(table);
  $("#" + this.mainTableId + " tbody").append(rows);
};

HatoholPriviledgeEditDialog.prototype.generateMainTable = function() {
  var html =
  '<table class="table table-condensed table-striped table-hover" id=' +
  this.mainTableId + '>' +
  '  <thead>' +
  '    <tr>' +
  '      <th>' + gettext("Allow") + '</th>' +
  '      <th>ID</th>' +
  '      <th>' + gettext("Type") + '</th>' +
  '      <th>' + gettext("Hostname") + '</th>' +
  '      <th>' + gettext("IP Address") + '</th>' +
  '      <th>' + gettext("Nickname") + '</th>' +
  '    </tr>' +
  '  </thead>' +
  '  <tbody></tbody>' +
  '</table>';
  return html;
};

HatoholPriviledgeEditDialog.prototype.generateTableRows = function() {
  var s = '';
  var servers = this.serversData.servers;
  for (var i = 0; i < servers.length; i++) {
    sv = servers[i];
    s += '<tr>';
    s += '<td><input type="checkbox" class="selectcheckbox" ' +
               'serverId="' + sv['id'] + '"></td>';
    s += '<td>' + sv.id + '</td>';
    s += '<td>' + makeMonitoringSystemTypeLabel(sv.type) + '</td>';
    s += '<td>' + sv.hostName + '</td>';
    s += '<td>' + sv.ipAddress + '</td>';
    s += '<td>' + sv.nickname  + '</td>';
    s += '</tr>';
  }
  return s;
};
