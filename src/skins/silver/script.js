/********************************************************************
**  Nulloy Music Player, http://nulloy.com
**  Copyright (C) 2010-2014 Sergey Vlasov <sergey@vlasov.me>
**
**  This skin package including all images, cascading style sheets,
**  UI forms, and JavaScript files are released under
**  Attribution-ShareAlike Unported License 3.0 (CC-BY-SA 3.0).
**  Please review the following information to ensure the CC-BY-SA 3.0
**  License requirements will be met:
**
**  http://creativecommons.org/licenses/by-sa/3.0/
**
*********************************************************************/

function Program(player)
{
	try {
		this.player = player;
		this.mainWindow = player.mainWindow();
		this.playbackEngine = player.playbackEngine();
		this.playlistWidget = this.mainWindow.findChild("playlistWidget");
		this.dropArea = this.mainWindow.findChild("dropArea");
		this.playButton = this.mainWindow.findChild("playButton");
		this.stopButton = this.mainWindow.findChild("stopButton");
		this.prevButton = this.mainWindow.findChild("prevButton");
		this.nextButton = this.mainWindow.findChild("nextButton");
		this.volumeSlider = this.mainWindow.findChild("volumeSlider");
		this.waveformSlider = this.mainWindow.findChild("waveformSlider");
		this.sizeGrip = this.mainWindow.findChild("sizeGrip");
		this.playlistToggleButton = this.mainWindow.findChild("playlistToggleButton");
		this.shadowWidget = this.mainWindow.findChild("shadowWidget");
		this.closeButton = this.mainWindow.findChild("closeButton");
		this.minimizeButton = this.mainWindow.findChild("minimizeButton");
		this.titleLabel = this.mainWindow.findChild("titleLabel");
		this.coverWidget = this.mainWindow.findChild("coverWidget");
		this.playlistContainer = this.mainWindow.findChild("playlistContainer");
		this.controlsContainer = this.mainWindow.findChild("controlsContainer");

		this.repeatCheckBox = this.mainWindow.findChild("repeatCheckBox");
		this.repeatCheckBox["clicked(bool)"].connect(this.playlistWidget["setRepeatMode(bool)"]);
		this.playlistWidget["repeatModeChanged(bool)"].connect(this.repeatCheckBox["setChecked(bool)"]);
		this.repeatCheckBox.setChecked(this.playlistWidget.repeatMode());

		this.shuffleCheckBox = this.mainWindow.findChild("shuffleCheckBox");
		this.shuffleCheckBox["clicked(bool)"].connect(this.playlistWidget["setShuffleMode(bool)"]);
		this.playlistWidget["shuffleModeChanged(bool)"].connect(this.shuffleCheckBox["setChecked(bool)"]);

		this.playButton.clicked.connect(this, "on_playButton_clicked");
		this.stopButton.clicked.connect(this.playbackEngine.stop);
		this.prevButton.clicked.connect(this.playlistWidget.playPreviousRow);
		this.nextButton.clicked.connect(this.playlistWidget.playNextRow);

		this.titleWidget = this.mainWindow.findChild("titleWidget");
		this.titleWidget.enableDoubleClick();
		this.titleWidget.doubleClicked.connect(this.mainWindow.toggleMaximize);

		this.volumeSlider.minimum = 0;
		this.volumeSlider.maximum = 100;

		this.waveformSlider.minimum = 0;
		this.waveformSlider.maximum = 10000;

		this.playbackEngine["stateChanged(N::PlaybackState)"].connect(this, "on_stateChanged");
		this.playbackEngine["mediaChanged(const QString &)"].connect(this.waveformSlider["drawFile(const QString &)"]);
		this.playbackEngine["mediaChanged(const QString &)"].connect(this.coverWidget["setSource(const QString &)"]);
		this.playbackEngine["finished()"].connect(this.playlistWidget.currentFinished);
		this.playbackEngine["failed()"].connect(this, "on_failed");
		this.playlistWidget["mediaSet(const QString &)"].connect(this.playbackEngine["setMedia(const QString &)"]);
		this.playlistWidget["currentActivated()"].connect(this.playbackEngine.play);

		this.volumeSlider["sliderMoved(qreal)"].connect(this.playbackEngine["setVolume(qreal)"]);
		this.playbackEngine["volumeChanged(qreal)"].connect(this.volumeSlider["setValue(qreal)"]);

		this.waveformSlider["sliderMoved(qreal)"].connect(this.playbackEngine["setPosition(qreal)"]);
		this.playbackEngine["positionChanged(qreal)"].connect(this.waveformSlider["setValue(qreal)"]);

		this.dropArea["filesDropped(const QStringList &)"].connect(this.playlistWidget["playFiles(const QStringList &)"]);
		this.mainWindow.windowFlags = (this.mainWindow.windowFlags | Qt.FramelessWindowHint | Qt.WindowCloseButtonHint) ^ (Qt.WindowTitleHint | Qt.Dialog);

		this.closeButton.clicked.connect(this.mainWindow.close);
		this.minimizeButton.clicked.connect(this.mainWindow.showMinimized);

		this.mainWindow["newTitle(const QString &)"].connect(this, "setTitle");
		this.mainWindow["fullScreenEnabled(bool)"].connect(this, "on_fullScreenEnabled");
		this.mainWindow.resized.connect(this, "on_resized");

		this.shadowWidget.setParent(this.dropArea);
		this.shadowWidget.setParent(this.playlistWidget);
		this.shadowWidget.show();

		this.splitter = this.mainWindow.findChild("splitter");
		this.splitter["splitterMoved(int, int)"].connect(this, "on_splitterMoved");

		if (Q_WS == "mac") {
			this.mainWindow.setAttribute(Qt.WA_MacBrushedMetal, true);

			var playlistWidget = this.mainWindow.findChild("playlistWidget");
			playlistWidget.setAttribute(Qt.WA_MacShowFocusRect, false);

			var borderWidget = this.mainWindow.findChild("borderWidget");
			borderWidget.layout().setContentsMargins(0, 0, 0, 0);
			borderWidget.styleSheet = "#borderWidget { background: #6e6e6e; }"

			var dropAreaMargins = this.dropArea.layout().contentsMargins();

			var titleLabel = this.mainWindow.findChild("titleLabel");
			titleLabel.setFontSize(12);

			this.sizeGrip.setParent(borderWidget);
		} else {
			this.sizeGrip.hide();
			this.mainWindow.setSizeGripEnabled(true);
		}

		if (WS_BUTTOS_SIDE == "left") {
			var titleBarlLayout = this.mainWindow.findChild("titleBarlLayout");
			titleBarlLayout.insertWidget(0, this.mainWindow.findChild("closeWrapperOuter"));
			titleBarlLayout.insertWidget(1, this.mainWindow.findChild("minimizeWrapperOuter"));
			titleBarlLayout.insertWidget(5, this.mainWindow.findChild("iconLabel"));
		}
	} catch (err) {
		print("QtScript: " + err);
	}
}

Program.prototype.afterShow = function()
{
	if (this.player.settings().value("SilverSkin/PlaylistVisible", true) == 'false') {
		this.player.settings().setValue("SilverSkin/PlaylistVisible", false);
		this.playlistWidget.hide();
		this.mainWindow.minimumHeight = this.mainWindow.maximumHeight = this.maximumHeight;
		this.mainWindow.resize(this.mainWindow.width, this.mainWindow.minimumHeigh);
	}

	if (this.player.settings().value("SilverSkin/Splitter"))
		this.splitter.setSizes(this.player.settings().value("SilverSkin/Splitter"));
}

Program.prototype.on_playButton_clicked = function()
{
	if (!this.playlistWidget.hasCurrent())
		this.playlistWidget.playRow(0);
	else
		this.playbackEngine.play(); // toggle play/pause
}

Program.prototype.on_stateChanged = function(state)
{
	if (state == N.PlaybackPlaying)
		this.playButton.styleSheet = "qproperty-icon: url(pause.png)";
	else
		this.playButton.styleSheet = "qproperty-icon: url(play.png)";

	this.waveformSlider.setPausedState(state == N.PlaybackPaused);
}

Program.prototype.on_failed = function()
{
	this.playlistWidget.currentFailed();
	this.playlistWidget.playNextRow();
}

Program.prototype.on_resized = function()
{
	if (Q_WS == "mac") {
		this.sizeGrip.move(this.sizeGrip.parentWidget().width -
		                   this.sizeGrip.width - 5,
		                   this.sizeGrip.parentWidget().height -
		                   this.sizeGrip.height - 4);
	}

	this.shadowWidget.resize(this.playlistWidget.width, this.shadowWidget.height);
}

Program.prototype.on_splitterMoved = function(pos, index)
{
	this.player.settings().setValue("SilverSkin/Splitter", this.splitter.sizes());
}

Program.prototype.setTitle = function(title)
{
	this.titleLabel.text = title;
	this.titleLabel.toolTip = title;
}

Program.prototype.on_fullScreenEnabled = function(enabled)
{
	this.playlistContainer.setVisible(!enabled);
	this.controlsContainer.setVisible(!enabled);
	this.titleWidget.setVisible(!enabled);
}

