console.log("Hello, World!")

const origin = window.location.origin

function setEmotion(emotion) {
	fetch(`${origin}/protogen/head/emotion`, {method: "put", body: emotion})
}

let emotion_options_container = document.getElementById("emotion-options-container")
for(const option of emotion_options_container.children){
	option.addEventListener("click", (e) => {
		setEmotion(option.value)
	})
}

