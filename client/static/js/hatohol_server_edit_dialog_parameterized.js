/*
 * Copyright (C) 2013-2014 Project Hatohol
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

var HatoholServerEditDialogParameterized = function(params) {
  var self = this;

  self.operator = params.operator;
  self.server = params.targetServer;
  self.succeededCallback = params.succeededCallback;
  self.windowTitle =
    self.server ? gettext("EDIT SERVER") : gettext("ADD SERVER");
  self.applyButtonTitle = self.server ? gettext("APPLY") : gettext("ADD");

  var dialogButtons = [{
    text: self.applyButtonTitle,
    click: addButtonClickedCb
  }, {
    text: gettext("CANCEL"),
    click: cancelButtonClickedCb
  }];

  // call the constructor of the super class
  var dialogAttrs = { width: "auto" };
  HatoholDialog.apply(
      this, ["server-edit-dialog", self.windowTitle,
             dialogButtons, dialogAttrs]);

  // set initial state
  if (self.server)
    self.setServer(self.server);
  self.fixupApplyButtonState();

  //
  // Dialog button handlers
  //
  function addButtonClickedCb() {
    if (validateParameters()) {
      if (self.server)
        hatoholInfoMsgBox(gettext("Now updating the server ..."));
      else
        hatoholInfoMsgBox(gettext("Now adding a server..."));
      postAddServer();
    }
  }

  function cancelButtonClickedCb() {
    self.closeDialog();
  }

  function makeQueryData() {
    var queryData = {};
    // TODO: implemnet
    return queryData;
  }

  function postAddServer() {
    // TODO: URL should be changed
    var url = "/server";
    if (self.server)
      url += "/" + self.server.id;
    new HatoholConnector({
      url: url,
      request: self.server ? "PUT" : "POST",
      data: makeQueryData(),
      replyCallback: replyCallback,
      parseErrorCallback: hatoholErrorMsgBoxForParser
    });
  }

  function replyCallback(reply, parser) {
    self.closeDialog();
    if (self.server)
      hatoholInfoMsgBox(gettext("Successfully updated."));
    else
      hatoholInfoMsgBox(gettext("Successfully created."));

    if (self.succeededCallback)
      self.succeededCallback();
  }

  function validateParameters() {
    // TODO: implement
    return true;
  }
};

HatoholServerEditDialogParameterized.prototype = Object.create(HatoholDialog.prototype);
HatoholServerEditDialogParameterized.prototype.constructor = HatoholServerEditDialogParameterized;

HatoholServerEditDialogParameterized.prototype.createMainElement = function() {
  var self = this;
  getServerTypesAsync();
  var div = $(makeMainDivHTMLForDynamicCreation());
  return div;

  function getServerTypesAsync() {
    new HatoholConnector({
      url: "/server-type",
      replyCallback: replyCallback,
      parseErrorCallback: hatoholErrorMsgBoxForParser
    });
  }

  function replyCallback(reply, parser) {
    if (!(reply.serverType instanceof Array)) {
      hatoholErrorMsgBox("[Malformed reply] Not found array: serverType");
      return;
    }
    self.paramArray = []
    for (var i = 0; i < reply.serverType.length; i ++) {
      var serverTypeInfo = reply.serverType[i];
      var name = serverTypeInfo.name;
      if (!name) {
        hatoholErrorMsgBox("[Malformed reply] Not found element: name");
        return;
      }
      var type = serverTypeInfo.type;
      if (type == undefined) {
        hatoholErrorMsgBox("[Malformed reply] Not found element: type");
        return;
      }

      var parameters = serverTypeInfo.parameters;
      if (parameters == undefined) {
        hatoholErrorMsgBox("[Malformed reply] Not found element: parameters");
        return;
      }

      $('#selectServerType').append($('<option>').html(name).val(type));
      self.paramArray[type] = parameters;
    }
  }

  function makeMainDivHTMLForDynamicCreation() {
    var s = '';
    s += '<div id="add-server-div">';
    s += '  <form class="form-inline">';
    s += '    <label>' + gettext("Server type") + '</label>';
    s += '    <select id="selectServerType" />';
    s += '  </form>';
    s += '  <form id="add-server-param-form" class="form-horizontal" role="form" />';
    s += '</div>';
    return s;
  }
};

HatoholServerEditDialogParameterized.prototype.onAppendMainElement = function () {
  var self = this;

  self.fixupApplyButtonState();

  $('#selectServerType').change(function() {
    var type = $("#selectServerType").val();
    $('#add-server-param-form').empty();
    setupParametersForms(self.paramArray[type]);
  });

  function setupParametersForms(parameters) {
    console.log(parameters);
    paramObj = JSON.parse(parameters);
    console.log(paramObj);
    if (!(paramObj instanceof Array)) {
        hatoholErrorMsgBox("[Malformed reply] parameters is not array");
        return;
    }

    var s = '';
    for (var i = 0; i < paramObj.length; i++)
      s += makeFormHTMLOfOneParameter(paramObj[i]);
    $('#add-server-param-form').append(s);
  }

  function makeFormHTMLOfOneParameter(param) {
    var s = '';
    var label = param.name;
    s += '<div class="form-group">';
    s += '  <label for="exampleInputPassword1" class="col-sm-4 control-label">' + gettext(label) + '</label>';
    s += '  <div class="col-sm-8">';
    s += '    <input type="text" class="form-control" id="exampleInputPassword1" placeholder="">';
    s += '  </div>'
    s += '</div>'
    return s;
  }
};

HatoholServerEditDialogParameterized.prototype.setApplyButtonState = function(state) {
  var btn = $(".ui-dialog-buttonpane").find("button:contains(" +
              this.applyButtonTitle + ")");
  if (state) {
     btn.removeAttr("disabled");
     btn.removeClass("ui-state-disabled");
  } else {
     btn.attr("disabled", "disable");
     btn.addClass("ui-state-disabled");
  }
};

HatoholServerEditDialogParameterized.prototype.fixupApplyButtonState = function(enable) {
  // TODO: implement
  var state = false;
  this.setApplyButtonState(state);
};

HatoholServerEditDialogParameterized.prototype.setServer = function(server) {
  this.server = server;
  // TODO: implement
  this.fixupApplyButtonState();
};
