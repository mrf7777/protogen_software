
const origin = window.location.origin

// emotions
function setEmotion(emotion) {
	fetch(`${origin}/protogen/head/emotion`, {method: "put", body: emotion})
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


// colors
function setMouthColor(color) {
	fetch(`${origin}/protogen/head/mouth/color`, {method: "put", body: color})
}

function setEyeColor(color) {
	fetch(`${origin}/protogen/head/eye/color`, {method: "put", body: color})
}

let mouth_color_input = document.getElementById("mouth-color")
mouth_color_input.addEventListener("input", (e) => {
	setMouthColor(e.target.value)
})

let eye_color_input = document.getElementById("eye-color")
eye_color_input.addEventListener("input", (e) => {
	setEyeColor(e.target.value)
})

// audio levels
function setAudioLevel(level) {
	fetch(`${origin}/protogen/head/audio-loudness`, {method: "put", body: level})
}

let audio_level_input = document.getElementById("audio-level")
audio_level_input.addEventListener("input", (e) => {
	setAudioLevel(e.target.value)
})
