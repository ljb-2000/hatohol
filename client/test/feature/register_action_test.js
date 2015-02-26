var x = require("casper").selectXPath;
var util = require("feature_test_utils");
casper.options.viewportSize = {width: 1024, height: 768};
casper.on("page.error", function(msg, trace) {
  this.echo("Error: " + msg, "ERROR");
  for(var i = 0; i < trace.length; i++) {
    var step = trace[i];
    this.echo("   " + step.file + " (line " + step.line + ")", "ERROR");
  }
});

casper.test.begin('Register/Unregister action test', function(test) {
  casper.start('http://0.0.0.0:8000/ajax_dashboard');
  casper.then(function() {util.login(test);});
  casper.then(function() {
    util.moveToServersPage(test);
    casper.then(function() {
      util.registerMonitoringServer(test,
                                    {serverType: 0,
                                     nickName: "test",
                                     serverName: "test",
                                     ipAddress: "127.0.0.1",
                                     userName: "admin",
                                     userPassword: "zabbix-admin"});
    });
  });
  // move to actions page
  casper.waitForSelector(x("//a[normalize-space(text())='アクション']"),
    function success() {
      test.assertExists(x("//a[normalize-space(text())='アクション']",
                          "Found 'actions' in menu"));
      this.click(x("//a[normalize-space(text())='アクション']"));
    },
    function fail() {
      test.assertExists(x("//a[normalize-space(text())='アクション']"));
    });
  casper.then(function() {
    casper.waitForUrl(/.*ajax_actions/, function() {
      test.assertUrlMatch(/.*ajax_actions/, "Move into actions page.");
    });
  });

  // create actions
  casper.waitForSelector("form button#add-action-button",
    function success() {
      test.assertExists("form button#add-action-button",
                        "Found add action button item");
      this.click("form button#add-action-button");
    },
    function fail() {
      test.assertExists("form button#add-action-button");
    });
  casper.then(function() {
    casper.evaluate(function() {
      $("#selectServerId").val("SELECT").change();
    });
  });
  casper.then(function() {
    this.evaluate(function() {
      $("#selectorMainTable").find("tr").last().click().change();
    });
  });
  casper.waitForSelector(".ui-dialog-buttonset > button",
    function success() {
      test.assertExists(".ui-dialog-buttonset > button",
                        "Confirmation dialog button appeared when " +
                        "registering target action.");
      this.evaluate(function() {
        $("div.ui-dialog-buttonset").attr("area-described-by","server-selector")
          .last().find("button").click().change();
      });
    },
    function fail() {
      test.assertExists(".ui-dialog-buttonset > button");
    });
  casper.waitForSelector("input#inputActionCommand",
    function success() {
      this.sendKeys("input#inputActionCommand", "getlog");
    },
    function fail() {
      test.assertExists("input#inputActionCommand");
    });
  casper.waitForSelector(".ui-dialog-buttonset > button",
    function success() {
      test.assertExists(".ui-dialog-buttonset > button",
                        "Confirmation dialog button appeared when " +
                        "registering action.");
      this.click(".ui-dialog-buttonset > button");
    },
    function fail() {
      test.assertExists(".ui-dialog-buttonset > button");
    });

  // check delete-selector check box in minitoring server
  casper.then(function() {
    casper.wait(200, function() {
      this.evaluate(function() {
        $("tr:last").find(".selectcheckbox").click();
        return true;
      });
    });
  });

  casper.waitForSelector("form button#delete-action-button",
    function success() {
      test.assertExists("form button#delete-action-button",
                        "Found delete action button.");
      this.click("form button#delete-action-button");
    },
    function fail() {
      test.assertExists("form button#delete-action-button");
    });

  // click Yes in delete action dialog
  casper.waitForSelector("div.ui-dialog-buttonset button",
    function success() {
      test.assertExists("div.ui-dialog-buttonset button",
                        "Confirmation dialog appeared.");
      this.evaluate(function() {
        $("div.ui-dialog-buttonset button").next().click();
      });
    },
    function fail() {
      test.assertExists("div.ui-dialog-buttonset button");
    });
  // close result confirmation dialog
  casper.waitForSelector("div.ui-dialog-buttonset > button",
    function success() {
      this.click("div.ui-dialog-buttonset > button");
    },
    function fail() {
      test.assertExists("div.ui-dialog-buttonset > button");
    });
  casper.then(function() {
    util.moveToServersPage(test);
    util.unregisterMonitoringServer();
  });
  casper.then(function() {util.logout(test);});
  casper.run(function() {test.done();});
});
