console.log("Hello, World!")

const origin = window.location.origin

// emotions
function setEmotion(emotion) {
	fetch(`${origin}/protogen/head/emotion`, {method: "put", body: emotion})
}

let emotion_options_container = document.getElementById("emotion-options-container")
for(const option of emotion_options_container.children){
	option.addEventListener("click", (e) => {
		setEmotion(option.value)
	})
}

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
