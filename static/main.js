
const origin = window.location.origin

// emotions
function setEmotion(emotion) {
	fetch(`${origin}/protogen/head/emotion`, {method: "put", body: emotion})
	renderEmotionAsCurrentEmotion(emotion)
}
function getCurrentEmotion(callback) {
	fetch(`${origin}/protogen/head/emotion`, {method: "get"})
		.then(response => response.text())
		.then(emotion_text => callback(emotion_text))
}
function getPossibleEmotions() {
	return fetch(`${origin}/protogen/head/emotion/all`, {method: "get"})
		.then(response => response.text())
		.then(text => {
			console.log(text)
			const emotions = text.split(/\r?\n/).filter(emotion => emotion !== "")
			console.log(emotions)
			return emotions
		})
		.catch(error => console.log(error))
}

let emotion_options_container = document.getElementById("emotion-options-container")
getPossibleEmotions().then(possible_emotions => {
	for(let emotion of possible_emotions) {
		let button = document.createElement("button")
		button.id = `${emotion}-button`
		button.value = emotion
		button.addEventListener("click", (e) => {
			setEmotion(emotion)
		});
		button.appendChild(document.createTextNode(emotion))

		let button_image = document.createElement("img")
		button_image.setAttribute("src", `${origin}/protogen/head/emotion/images/${emotion}.png`)
		button_image.setAttribute("alt", emotion)
		button.appendChild(button_image)

		emotion_options_container.appendChild(button)
	}
});

function renderEmotionAsCurrentEmotion(emotion) {
	const current_emotion_style_class = "current-emotion"
	// first, unstyle any button that may represent the current emotion
	let buttons_representing_current_emotion = document.querySelectorAll(`button[class=${current_emotion_style_class}]`)
	buttons_representing_current_emotion.forEach(button => {
		button.classList = ""
	})
	// find and style button that represent the current emotion
	let button_matching_current_emotion = document.querySelector(`button[value=${emotion}]`)
	button_matching_current_emotion.classList = "current-emotion"
}

function updateCurrentEmotion() {
	getCurrentEmotion((emotion) => {
		renderEmotionAsCurrentEmotion(emotion)
	})
}

updateCurrentEmotion()
const update_current_emotion_interval = setInterval(updateCurrentEmotion, 1000)

// screen blank
function setScreenBlank(blank) {
	fetch(`${origin}/protogen/head/blank`, {method: "put", body: blank})
}
function getScreenBlank(callback) {
	fetch(`${origin}/protogen/head/blank`, {method: "get"})
		.then(response => response.text())
		.then(blank_text => callback(blank_text))

}

let screen_blank_radio_container = document.getElementById("screen-blank")
screen_blank_radio_container.addEventListener("change", (e) => {
	setScreenBlank(e.target.value)
})

function updateScreenBlank() {
	getScreenBlank(blank_text => {
		let radio_to_check = screen_blank_radio_container.querySelector(`input[name=screen-blank][value=${blank_text}]`)
		radio_to_check.checked = true
	})
}

updateScreenBlank()
const update_screen_blank = setInterval(updateScreenBlank, 1000)

// audio levels
function setAudioLevel(level) {
	fetch(`${origin}/protogen/head/audio-loudness`, {method: "put", body: level})
}

let audio_level_input = document.getElementById("audio-level")
audio_level_input.addEventListener("input", (e) => {
	setAudioLevel(e.target.value)
})
